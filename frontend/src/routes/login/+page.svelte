<script lang="ts">
	import { auth } from '$lib/stores/auth';
	import { goto } from '$app/navigation';

	let email = $state('');
	let password = $state('');
	let error = $state('');
	let loading = $state(false);

	async function handleSubmit(e: Event) {
		e.preventDefault();
		error = '';
		loading = true;
		try {
			await auth.login(email, password);
			goto('/me/bots');
		} catch (err) {
			error = err instanceof Error ? err.message : 'Login failed';
		} finally {
			loading = false;
		}
	}
</script>

<svelte:head>
	<title>Login - GameOfLifes</title>
</svelte:head>

<div class="auth-page">
	<div class="auth-card card">
		<h1>Welcome back</h1>
		<p class="auth-subtitle">Sign in to manage your bots and watch them evolve.</p>

		<form onsubmit={handleSubmit}>
			<div class="field">
				<label for="email">Email</label>
				<input id="email" type="email" bind:value={email} required placeholder="you@example.com" />
			</div>
			<div class="field">
				<label for="password">Password</label>
				<input
					id="password"
					type="password"
					bind:value={password}
					required
					placeholder="Your password"
				/>
			</div>

			{#if error}
				<p class="error-text">{error}</p>
			{/if}

			<button type="submit" class="btn-primary submit-btn" disabled={loading}>
				{loading ? 'Signing in...' : 'Sign in'}
			</button>
		</form>

		<p class="auth-footer">No account? <a href="/signup">Create one</a></p>
	</div>
</div>

<style>
	.auth-page {
		display: flex;
		justify-content: center;
		align-items: center;
		min-height: calc(100vh - var(--nav-height));
		padding: var(--sp-4);
	}

	.auth-card {
		width: 100%;
		max-width: 420px;
		animation: fade-in 0.3s ease forwards;
	}

	.auth-card h1 {
		font-size: var(--text-2xl);
		margin-bottom: var(--sp-1);
	}

	.auth-subtitle {
		color: var(--text-muted);
		margin-bottom: var(--sp-6);
	}

	.field {
		margin-bottom: var(--sp-4);
	}

	.submit-btn {
		width: 100%;
		padding: var(--sp-3);
		font-size: var(--text-base);
		margin-top: var(--sp-2);
	}

	.auth-footer {
		text-align: center;
		margin-top: var(--sp-6);
		margin-bottom: 0;
		font-size: var(--text-sm);
	}
</style>
