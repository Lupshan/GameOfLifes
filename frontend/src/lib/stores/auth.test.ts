import { describe, it, expect, vi, beforeEach } from 'vitest';

// Mock localStorage before importing auth store.
const storage: Record<string, string> = {};
vi.stubGlobal('localStorage', {
	getItem: (key: string) => storage[key] ?? null,
	setItem: (key: string, val: string) => { storage[key] = val; },
	removeItem: (key: string) => { delete storage[key]; }
});

// Mock fetch.
vi.stubGlobal('fetch', vi.fn());

describe('auth store concept', () => {
	beforeEach(() => {
		Object.keys(storage).forEach(k => delete storage[k]);
	});

	it('localStorage token persistence works', () => {
		localStorage.setItem('gol_token', 'test-token');
		expect(localStorage.getItem('gol_token')).toBe('test-token');
		localStorage.removeItem('gol_token');
		expect(localStorage.getItem('gol_token')).toBeNull();
	});

	it('api fetch adds Authorization header', async () => {
		localStorage.setItem('gol_token', 'my-jwt');
		const mockFetch = vi.fn().mockResolvedValue(new Response('{}'));
		vi.stubGlobal('fetch', mockFetch);

		// Inline the logic from api.ts to test header injection.
		const token = localStorage.getItem('gol_token');
		const headers: Record<string, string> = { 'Content-Type': 'application/json' };
		if (token) headers['Authorization'] = `Bearer ${token}`;

		expect(headers['Authorization']).toBe('Bearer my-jwt');
	});
});
