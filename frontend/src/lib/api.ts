/** API fetch wrapper — sends credentials (httpOnly cookie) automatically. */

const API_BASE = import.meta.env.VITE_API_URL || '/api';

export async function apiFetch(path: string, options: RequestInit = {}): Promise<Response> {
	const headers: Record<string, string> = {
		'Content-Type': 'application/json',
		...(options.headers as Record<string, string>)
	};

	return fetch(`${API_BASE}${path}`, {
		...options,
		headers,
		credentials: 'include'
	});
}

/** Create a bot from source code. */
export async function createBot(
	name: string,
	source: string
): Promise<{
	id: string;
	compile_ok: boolean;
	compile_errors?: { message: string; line: number; col: number }[];
}> {
	const resp = await apiFetch('/bots', {
		method: 'POST',
		body: JSON.stringify({ name, source })
	});
	if (!resp.ok && resp.status !== 201) {
		const err = await resp.json().catch(() => ({}));
		throw new Error(err.detail || 'Failed to create bot');
	}
	return resp.json();
}

/** Publish a bot. */
export async function publishBot(botId: string): Promise<void> {
	const resp = await apiFetch(`/bots/${botId}/publish`, { method: 'POST' });
	if (!resp.ok) {
		const err = await resp.json();
		throw new Error(err.detail || 'Failed to publish');
	}
}

/** Delete a bot. */
export async function deleteBot(botId: string): Promise<void> {
	const resp = await apiFetch(`/bots/${botId}`, { method: 'DELETE' });
	if (!resp.ok) {
		const err = await resp.json();
		throw new Error(err.detail || 'Failed to delete');
	}
}

/** Get bot detail. */
export async function getBot(botId: string): Promise<{
	id: string;
	name: string;
	published: boolean;
	compile_ok: boolean;
	compile_errors?: { message: string; line: number; col: number }[];
	source: string;
}> {
	const resp = await apiFetch(`/bots/${botId}`);
	if (!resp.ok) throw new Error('Bot not found');
	return resp.json();
}
