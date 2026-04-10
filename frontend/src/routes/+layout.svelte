<script lang="ts">
	import { auth } from '$lib/stores/auth';
	import { onMount } from 'svelte';
	import type { Snippet } from 'svelte';

	let { children }: { children: Snippet } = $props();

	let user = $derived($auth.user);
	let token = $derived($auth.token);

	onMount(() => {
		if ($auth.token && !$auth.user) {
			auth.fetchUser();
		}
	});
</script>

<nav style="display:flex;gap:1rem;padding:0.5rem 1rem;background:#222;color:#fff;align-items:center;">
	<a href="/" style="color:#fff;text-decoration:none;font-weight:bold;">GameOfLifes</a>
	<a href="/world" style="color:#ccc;text-decoration:none;">World</a>
	<a href="/replay" style="color:#ccc;text-decoration:none;">Replay</a>
	<a href="/stats" style="color:#ccc;text-decoration:none;">Stats</a>
	{#if token}
		<a href="/edit" style="color:#ccc;text-decoration:none;">Editor</a>
		<a href="/me/bots" style="color:#ccc;text-decoration:none;">My Bots</a>
		<span style="margin-left:auto;color:#aaa;">{user?.email ?? ''}</span>
		<button onclick={() => { auth.logout(); window.location.href = '/'; }}
			style="background:none;border:1px solid #666;color:#ccc;padding:0.25rem 0.5rem;cursor:pointer;">
			Logout
		</button>
	{:else}
		<a href="/login" style="margin-left:auto;color:#ccc;text-decoration:none;">Login</a>
		<a href="/signup" style="color:#ccc;text-decoration:none;">Sign up</a>
	{/if}
</nav>

<main>
	{@render children()}
</main>
