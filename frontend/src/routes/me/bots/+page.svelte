<script lang="ts">
	import { auth } from '$lib/stores/auth';
	import { apiFetch } from '$lib/api';
	import { onMount } from 'svelte';
	import { goto } from '$app/navigation';

	interface Bot {
		id: string;
		name: string;
		published: boolean;
		compile_ok: boolean;
	}

	let bots = $state<Bot[]>([]);
	let loading = $state(true);

	onMount(async () => {
		if (!$auth.token) {
			goto('/login');
			return;
		}
		const resp = await apiFetch('/bots');
		if (resp.ok) {
			bots = await resp.json();
		}
		loading = false;
	});
</script>

<svelte:head>
	<title>My Bots - GameOfLifes</title>
</svelte:head>

<div style="max-width:800px;margin:2rem auto;padding:1rem;">
	<h1>My Bots</h1>

	{#if loading}
		<p>Loading...</p>
	{:else if bots.length === 0}
		<p>No bots yet. <a href="/edit">Create one</a></p>
	{:else}
		<table style="width:100%;border-collapse:collapse;">
			<thead>
				<tr style="text-align:left;border-bottom:2px solid #ccc;">
					<th style="padding:0.5rem;">Name</th>
					<th style="padding:0.5rem;">Compiled</th>
					<th style="padding:0.5rem;">Published</th>
					<th style="padding:0.5rem;">Actions</th>
				</tr>
			</thead>
			<tbody>
				{#each bots as bot}
					<tr style="border-bottom:1px solid #eee;">
						<td style="padding:0.5rem;">{bot.name}</td>
						<td style="padding:0.5rem;">{bot.compile_ok ? '✓' : '✗'}</td>
						<td style="padding:0.5rem;">{bot.published ? '✓' : '—'}</td>
						<td style="padding:0.5rem;">
							<a href="/me/bots/{bot.id}">View</a>
						</td>
					</tr>
				{/each}
			</tbody>
		</table>
	{/if}

	<p style="margin-top:1rem;"><a href="/edit">+ New Bot</a></p>
</div>
