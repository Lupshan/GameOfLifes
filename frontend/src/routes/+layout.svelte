<script lang="ts">
	import '../app.css';
	import { auth } from '$lib/stores/auth';
	import { onMount } from 'svelte';
	import type { Snippet } from 'svelte';

	let { children }: { children: Snippet } = $props();

	let user = $derived($auth.user);
	let loggedIn = $derived($auth.loggedIn);

	onMount(() => {
		if (!$auth.user) {
			auth.fetchUser();
		}
	});
</script>

<nav class="nav">
	<div class="nav-inner">
		<a href="/" class="nav-brand">GameOfLifes</a>

		<div class="nav-links">
			<a href="/world">World</a>
			<a href="/replay">Replay</a>
			<a href="/stats">Stats</a>
			<a href="/tournaments">Tournaments</a>
			<a href="/docs">Docs</a>
			{#if loggedIn}
				<a href="/edit">Editor</a>
				<a href="/me/bots">My Bots</a>
				<a href="/sandbox">Sandbox</a>
			{/if}
		</div>

		<div class="nav-auth">
			{#if loggedIn}
				<span class="nav-email">{user?.email ?? ''}</span>
				<button
					class="btn-ghost"
					onclick={() => {
						auth.logout();
						window.location.href = '/';
					}}
				>
					Logout
				</button>
			{:else}
				<a href="/login" class="btn-ghost">Login</a>
				<a href="/signup" class="btn btn-primary">Sign up</a>
			{/if}
		</div>
	</div>
</nav>

<main>
	{@render children()}
</main>

<style>
	.nav {
		position: sticky;
		top: 0;
		z-index: 100;
		height: var(--nav-height);
		background: var(--bg-surface);
		border-bottom: 1px solid var(--border);
		backdrop-filter: blur(12px);
	}

	.nav-inner {
		max-width: 1400px;
		margin: 0 auto;
		display: flex;
		align-items: center;
		gap: var(--sp-4);
		padding: 0 var(--sp-4);
		height: 100%;
	}

	.nav-brand {
		font-weight: 700;
		font-size: var(--text-lg);
		color: var(--accent) !important;
		text-decoration: none !important;
		letter-spacing: -0.02em;
		flex-shrink: 0;
	}

	.nav-links {
		display: flex;
		gap: var(--sp-1);
		margin-left: var(--sp-4);
	}

	.nav-links a {
		padding: var(--sp-1) var(--sp-3);
		border-radius: var(--radius);
		font-size: var(--text-sm);
		font-weight: 500;
		color: var(--text-secondary);
		text-decoration: none;
		transition: all var(--transition-fast);
	}

	.nav-links a:hover {
		color: var(--text-primary);
		background: var(--bg-raised);
		text-decoration: none;
	}

	.nav-auth {
		margin-left: auto;
		display: flex;
		align-items: center;
		gap: var(--sp-2);
	}

	.nav-email {
		font-size: var(--text-sm);
		color: var(--text-muted);
	}

	.nav-auth .btn-ghost {
		font-size: var(--text-sm);
		padding: var(--sp-1) var(--sp-3);
		color: var(--text-secondary);
		text-decoration: none;
	}

	.nav-auth .btn-primary {
		font-size: var(--text-sm);
		padding: var(--sp-1) var(--sp-3);
		text-decoration: none;
	}

	main {
		min-height: calc(100vh - var(--nav-height));
	}

	@media (max-width: 768px) {
		.nav-inner {
			gap: var(--sp-2);
			padding: 0 var(--sp-2);
		}

		.nav-links {
			gap: 0;
			margin-left: var(--sp-2);
			overflow-x: auto;
		}

		.nav-links a {
			padding: var(--sp-1) var(--sp-2);
			font-size: var(--text-xs);
		}

		.nav-email {
			display: none;
		}
	}
</style>
