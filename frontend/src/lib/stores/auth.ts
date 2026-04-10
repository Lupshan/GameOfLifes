/** Auth store: manages user state.
 *
 * Tokens are stored in httpOnly cookies set by the backend — the frontend
 * never touches the token directly, eliminating XSS token theft.
 */
import { writable } from 'svelte/store';
import { apiFetch } from '$lib/api';

interface User {
	id: string;
	email: string;
}

interface AuthState {
	user: User | null;
	loggedIn: boolean;
}

function createAuthStore() {
	const initial: AuthState = { user: null, loggedIn: false };
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
			// Token is set as httpOnly cookie by the backend response.
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
			// Token is set as httpOnly cookie by the backend response.
			await this.fetchUser();
		},

		async fetchUser(): Promise<void> {
			const resp = await apiFetch('/auth/me');
			if (resp.ok) {
				const user = await resp.json();
				set({ user, loggedIn: true });
			} else {
				set({ user: null, loggedIn: false });
			}
		},

		async logout(): Promise<void> {
			await apiFetch('/auth/logout', { method: 'POST' });
			set({ user: null, loggedIn: false });
		}
	};
}

export const auth = createAuthStore();
