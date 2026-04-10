<script lang="ts">
	import { onMount } from 'svelte';
	import { apiFetch } from '$lib/api';

	interface Tournament {
		id: string;
		name: string;
		description: string;
		status: string;
		max_ticks: number;
		scoring: string;
		leaderboard: { bot_name: string; score: number }[];
	}

	let tournaments = $state<Tournament[]>([]);
	let loading = $state(true);

	onMount(async () => {
		const resp = await apiFetch('/tournaments');
		if (resp.ok) {
			tournaments = await resp.json();
		}
		loading = false;
	});
</script>

<svelte:head>
	<title>Tournaments - GameOfLifes</title>
</svelte:head>

<div style="max-width:800px;margin:2rem auto;padding:1rem;">
	<h1>Tournaments</h1>

	{#if loading}
		<p>Loading...</p>
	{:else if tournaments.length === 0}
		<p>No tournaments yet.</p>
	{:else}
		{#each tournaments as t}
			<div style="border:1px solid #ddd;padding:1rem;margin-bottom:1rem;border-radius:4px;">
				<h3>{t.name}</h3>
				<p>{t.description}</p>
				<p>Status: {t.status} | Scoring: {t.scoring} | Max ticks: {t.max_ticks}</p>

				{#if t.leaderboard.length > 0}
					<h4>Leaderboard</h4>
					<ol>
						{#each t.leaderboard as entry}
							<li>{entry.bot_name}: {entry.score}</li>
						{/each}
					</ol>
				{:else}
					<p style="color:#888;">No results yet.</p>
				{/if}
			</div>
		{/each}
	{/if}
</div>
