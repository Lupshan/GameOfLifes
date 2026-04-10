/** API fetch wrapper — adds Bearer token if available. */

const API_BASE = import.meta.env.VITE_API_URL || 'http://localhost:8000';

export async function apiFetch(path: string, options: RequestInit = {}): Promise<Response> {
	const token =
		typeof localStorage !== 'undefined' ? localStorage.getItem('gol_token') : null;

	const headers: Record<string, string> = {
		'Content-Type': 'application/json',
		...(options.headers as Record<string, string>)
	};

	if (token) {
		headers['Authorization'] = `Bearer ${token}`;
	}

	return fetch(`${API_BASE}${path}`, {
		...options,
		headers
	});
}
