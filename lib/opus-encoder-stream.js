var util = require('util');
var Transform = require('stream').Transform;
var ogg_packet = require('./binding').ogg_packet;

// These are the valid rates for libopus according to
// https://www.opus-codec.org/docs/opus_api-1.1.2/group__opus__encoder.html#gaa89264fd93c9da70362a0c9b96b9ca88
var VALID_RATES = [8000, 12000, 16000, 24000, 48000];

var Encoder = function(rate, channels, frameSize) {
  Transform.call(this, { readableObjectMode: true });

  this.rate = rate || 48000;

  // Ensure the range is valid.
  if (VALID_RATES.indexOf(this.rate) === -1) {
    throw new RangeError(
      'Encoder rate (' +
        this.rate +
        ') is not valid. ' +
        'Valid rates are: ' +
        VALID_RATES.join(', ')
    );
  }

  this.channels = channels || 1;
  this.frameSize = frameSize || this.rate * 0.04;

  this.headerWritten = false;
  this.pos = 0;
  this.granulepos = 0;
  this.samplesWritten = 0;
};
util.inherits(Encoder, Transform);

/**
 * _transform stream callback
 */
Encoder.prototype._transform = function(buf, encoding, done) {
  // Transform the buffer
  this._pushEncodedBuffer(buf);

  done();
};

Encoder.prototype._writeHeader = function() {
  // OpusHead packet
  var magicSignature = Buffer.from('OpusHead', 'ascii');
  var data = Buffer.from([
    0x01, // version
    this.channels,
    0x00,
    0x0f, // Preskip (default and recommended 3840)
    (this.rate & 0x000000ff) >> 0,
    (this.rate & 0x0000ff00) >> 8,
    (this.rate & 0x00ff0000) >> 16,
    (this.rate & 0xff000000) >> 24,
    0x00,
    0x00, // gain
    0x00 // Channel mappign (RTP, mono/stereo)
  ]);

  var header = Buffer.concat([magicSignature, data]);

  var packet = new ogg_packet();
  packet.packet = header;
  // packet.bytes = header.length;
  packet.b_o_s = 1;
  packet.e_o_s = 0;
  packet.granulepos = -1;
  packet.packetno = this.pos++;

  this.push(packet);

  // OpusTags packet
  magicSignature = Buffer.from('OpusTags', 'ascii');
  var vendor = Buffer.from('node-opus', 'ascii');
  var vendorLength = Buffer.alloc(4);
  vendorLength.writeUInt32LE(vendor.length, 0);
  var commentLength = Buffer.alloc(4);
  commentLength.writeUInt32LE(0, 0);

  header = Buffer.concat([
    magicSignature,
    vendorLength,
    vendor,
    commentLength,
    Buffer.from([0xff])
  ]);

  packet = new ogg_packet();
  packet.packet = header;
  packet.b_o_s = 0;
  packet.e_o_s = 0;
  packet.granulepos = -1;
  packet.packetno = this.pos++;
  packet.flush = true;

  this.push(packet);

  this.headerWritten = true;
};

Encoder.prototype._pushEncodedBuffer = function(encoded, end) {
  // Write the header if it hasn't been written yet
  if (!this.headerWritten) {
    this._writeHeader();
  }

  if (this.lastPacket) {
    this.push(this.lastPacket);
  }

  // Scale the frame size into 48 kHz bitrate, which is used for the
  // granule positioning. We'll still update the samplesWritten just to
  // ensure backwards compatibility.
  this.granulepos += (this.frameSize / this.rate) * 48000;
  this.samplesWritten += this.frameSize;

  var packet = new ogg_packet();
  packet.packet = encoded;
  packet.e_o_s = 0;
  packet.granulepos = this.granulepos;
  packet.packetno = this.pos++;
  packet.flush = true;

  this.lastPacket = packet;
};

Encoder.prototype._flush = function(done) {
  if (this.lastPacket) {
    this.lastPacket.e_o_s = 1;
    this.push(this.lastPacket);
  }

  done();
};

module.exports = Encoder;
