import { describe, it, expect, vi, beforeEach } from 'vitest';

// Mock fetch globally.
const mockFetch = vi.fn();
vi.stubGlobal('fetch', mockFetch);

describe('auth store (httpOnly cookies)', () => {
	beforeEach(() => {
		mockFetch.mockReset();
	});

	it('apiFetch sends credentials: include for cookie auth', async () => {
		mockFetch.mockResolvedValue(new Response('{}'));

		const { apiFetch } = await import('$lib/api');
		await apiFetch('/auth/me');

		expect(mockFetch).toHaveBeenCalledTimes(1);
		const [, options] = mockFetch.mock.calls[0];
		expect(options.credentials).toBe('include');
	});

	it('apiFetch does not add Authorization header', async () => {
		mockFetch.mockResolvedValue(new Response('{}'));

		const { apiFetch } = await import('$lib/api');
		await apiFetch('/auth/me');

		const [, options] = mockFetch.mock.calls[0];
		expect(options.headers['Authorization']).toBeUndefined();
	});
});
