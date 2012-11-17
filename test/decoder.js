
var fs = require('fs');
var path = require('path');
var assert = require('assert');
var Decoder = require('../').Decoder;
var fixtures = path.resolve(__dirname, 'fixtures');

describe('Decoder', function () {

  describe('"320x240.ogv" fixture file', function () {
    var fixture = path.resolve(fixtures, '320x240.ogv');

    it('should get 2 "stream" events', function (done) {
      var decoder = new Decoder();
      var input = fs.createReadStream(fixture);
      var count = 0;
      var expected = 2;
      decoder.on('stream', function () {
        count++;
      });
      decoder.on('finish', function () {
        assert.equal(expected, count);
        done();
      });
      input.pipe(decoder);
    });

    it('should get 1 "bos" event for each "stream"', function (done) {
      var decoder = new Decoder();
      var input = fs.createReadStream(fixture);
      var streamCount = 0;
      var bosCount = 0;
      decoder.on('stream', function (stream) {
        streamCount++;
        stream.on('bos', function () {
          bosCount++;
        });
      });
      decoder.on('finish', function () {
        assert.equal(streamCount, bosCount);
        done();
      });
      input.pipe(decoder);
    });

    it('should get 1 "eos" event for each "stream"', function (done) {
      var decoder = new Decoder();
      var input = fs.createReadStream(fixture);
      var streamCount = 0;
      var eosCount = 0;
      decoder.on('stream', function (stream) {
        streamCount++;
        stream.on('eos', function () {
          eosCount++;
        });
      });
      decoder.on('finish', function () {
        assert.equal(streamCount, eosCount);
        done();
      });
      input.pipe(decoder);
    });

    it('should get 1761486570 and 252396615 as stream serial numbers', function (done) {
      var decoder = new Decoder();
      var input = fs.createReadStream(fixture);
      var serials = [ 1761486570, 252396615 ];
      var seen = [];
      decoder.on('stream', function (stream) {
        assert.notEqual(-1, serials.indexOf(stream.serialno));
        seen.push(stream.serialno);
      });
      decoder.on('finish', function () {
        assert.deepEqual(serials, seen);
        done();
      });
      input.pipe(decoder);
    });

  });

});
