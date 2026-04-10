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

<div style="max-width:400px;margin:2rem auto;padding:1rem;">
	<h1>Login</h1>
	<form onsubmit={handleSubmit}>
		<div style="margin-bottom:1rem;">
			<label for="email">Email</label><br />
			<input id="email" type="email" bind:value={email} required
				style="width:100%;padding:0.5rem;" />
		</div>
		<div style="margin-bottom:1rem;">
			<label for="password">Password</label><br />
			<input id="password" type="password" bind:value={password} required
				style="width:100%;padding:0.5rem;" />
		</div>
		{#if error}
			<p style="color:red;">{error}</p>
		{/if}
		<button type="submit" disabled={loading}
			style="padding:0.5rem 1rem;cursor:pointer;">
			{loading ? 'Logging in...' : 'Login'}
		</button>
	</form>
	<p style="margin-top:1rem;">No account? <a href="/signup">Sign up</a></p>
</div>
