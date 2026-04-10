/** Decode base64-encoded grid data from snapshots. */

/** Decode base64 string to Uint8Array. */
export function decodeBase64(b64: string): Uint8Array {
	const binary = atob(b64);
	const bytes = new Uint8Array(binary.length);
	for (let i = 0; i < binary.length; i++) {
		bytes[i] = binary.charCodeAt(i);
	}
	return bytes;
}

/** Unpack a bitfield (MSB-first) into a flat boolean-like array. */
export function unpackBits(packed: Uint8Array, totalCells: number): Uint8Array {
	const cells = new Uint8Array(totalCells);
	for (let i = 0; i < totalCells; i++) {
		const byteIdx = Math.floor(i / 8);
		const bitIdx = 7 - (i % 8);
		if (byteIdx < packed.length) {
			cells[i] = (packed[byteIdx] >> bitIdx) & 1;
		}
	}
	return cells;
}

/** Decode a base64 bitfield grid (food, water, mineral). */
export function decodeBitGrid(b64: string, totalCells: number): Uint8Array {
	return unpackBits(decodeBase64(b64), totalCells);
}

/** Decode terrain grid (raw bytes, not bit-packed). */
export function decodeTerrainGrid(b64: string): Uint8Array {
	return decodeBase64(b64);
}
