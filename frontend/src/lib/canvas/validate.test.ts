import { describe, it, expect } from 'vitest';
import { isValidSnapshot } from './validate';

describe('isValidSnapshot', () => {
	it('returns true for valid snapshot', () => {
		const snap = {
			version: 2,
			tick: 42,
			width: 64,
			height: 64,
			seed: 1,
			agents: [],
			food: 'AAAA',
			water: 'AAAA',
			mineral: 'AAAA',
			terrain: 'AAAA'
		};
		expect(isValidSnapshot(snap)).toBe(true);
	});

	it('returns false for null', () => {
		expect(isValidSnapshot(null)).toBe(false);
	});

	it('returns false for missing fields', () => {
		expect(isValidSnapshot({ tick: 1 })).toBe(false);
	});

	it('returns false for wrong types', () => {
		expect(
			isValidSnapshot({
				version: '2',
				tick: 42,
				width: 64,
				height: 64,
				agents: [],
				food: 'a',
				water: 'a',
				mineral: 'a',
				terrain: 'a'
			})
		).toBe(false);
	});
});
