/** Auth store: manages token and user state. */
import { writable, get } from 'svelte/store';
import { apiFetch } from '$lib/api';

interface User {
	id: string;
	email: string;
}

interface AuthState {
	token: string | null;
	user: User | null;
}

function createAuthStore() {
	const initial: AuthState = {
		token: typeof localStorage !== 'undefined' ? localStorage.getItem('gol_token') : null,
		user: null
	};

	const { subscribe, set, update } = writable<AuthState>(initial);

	return {
		subscribe,

		async signup(email: string, password: string): Promise<void> {
			const resp = await apiFetch('/auth/signup', {
				method: 'POST',
				body: JSON.stringify({ email, password })
			});
			if (!resp.ok) {
				const err = await resp.json();
				throw new Error(err.detail || 'Signup failed');
			}
			const data = await resp.json();
			localStorage.setItem('gol_token', data.access_token);
			set({ token: data.access_token, user: null });
			await this.fetchUser();
		},

		async login(email: string, password: string): Promise<void> {
			const resp = await apiFetch('/auth/login', {
				method: 'POST',
				body: JSON.stringify({ email, password })
			});
			if (!resp.ok) {
				const err = await resp.json();
				throw new Error(err.detail || 'Login failed');
			}
			const data = await resp.json();
			localStorage.setItem('gol_token', data.access_token);
			set({ token: data.access_token, user: null });
			await this.fetchUser();
		},

		async fetchUser(): Promise<void> {
			const state = get({ subscribe });
			if (!state.token) return;
			const resp = await apiFetch('/auth/me');
			if (resp.ok) {
				const user = await resp.json();
				update((s) => ({ ...s, user }));
			}
		},

		logout(): void {
			localStorage.removeItem('gol_token');
			set({ token: null, user: null });
		}
	};
}

export const auth = createAuthStore();
