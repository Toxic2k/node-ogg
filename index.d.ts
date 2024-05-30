import { Readable, Writable } from 'stream'

declare class EncoderStream extends Writable {
    packetin(chunk: any, callback?: (error: Error | null | undefined) => void): boolean;
    packetin(chunk: any, encoding: BufferEncoding, callback?: (error: Error | null | undefined) => void): boolean;
    pageout(callback?: (error: Error | null | undefined) => void): boolean;
    flush(callback?: (error: Error | null | undefined) => void): boolean;
}

export class Encoder extends Readable implements NodeJS.ReadableStream {
    stream: (serialno:number|undefined) => EncoderStream
}

type PacketEventType = "packet";
type PageEventType = "page";
type EosEventType = "eos";

declare class DecoderStream extends Readable {
    // @ts-ignore
    on(name: PacketEventType, handler : (packet:ogg_packet) => void):this;
    // @ts-ignore
    on(name: PageEventType, handler : (page:any) => void):this;
    // @ts-ignore
    on(name: EosEventType, handler : () => void):this;
}

type StreamEventType = "stream";

export class Decoder extends Writable implements NodeJS.WritableStream {
    stream: (serialno:number|undefined) => DecoderStream
    // @ts-ignore
    on(name: StreamEventType, handler : (stream: DecoderStream) => void):this;
}

export class ogg_packet {
    packet: Buffer;
    bytes: number;
    b_o_s: 1|0;
    e_o_s: 1|0;
    granulepos: number;
    packetno: number;
}
