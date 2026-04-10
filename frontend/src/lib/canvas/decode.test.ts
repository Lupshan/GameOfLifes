import { describe, it, expect } from 'vitest';
import { decodeBase64, unpackBits, decodeBitGrid } from './decode';

describe('decodeBase64', () => {
	it('decodes a simple base64 string', () => {
		// "AQID" = bytes [1, 2, 3]
		const result = decodeBase64('AQID');
		expect(result).toEqual(new Uint8Array([1, 2, 3]));
	});

	it('decodes empty string', () => {
		const result = decodeBase64('');
		expect(result.length).toBe(0);
	});
});

describe('unpackBits', () => {
	it('unpacks a byte into 8 bits MSB first', () => {
		const packed = new Uint8Array([0b10110000]);
		const result = unpackBits(packed, 8);
		expect(Array.from(result)).toEqual([1, 0, 1, 1, 0, 0, 0, 0]);
	});

	it('handles partial last byte', () => {
		const packed = new Uint8Array([0xff]);
		const result = unpackBits(packed, 4);
		expect(Array.from(result)).toEqual([1, 1, 1, 1]);
	});
});

describe('decodeBitGrid', () => {
	it('decodes a base64 bitfield', () => {
		// 0xFF = all ones → 8 cells all set
		const b64 = btoa(String.fromCharCode(0xff));
		const result = decodeBitGrid(b64, 8);
		expect(result.every((v) => v === 1)).toBe(true);
	});
});
