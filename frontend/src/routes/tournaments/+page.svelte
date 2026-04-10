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

<div class="container">
	<h1>Tournaments</h1>

	{#if loading}
		<p class="loading">Loading tournaments...</p>
	{:else if tournaments.length === 0}
		<div class="card empty-state">
			<p>No tournaments yet. Check back soon!</p>
		</div>
	{:else}
		<div class="tournament-grid">
			{#each tournaments as t}
				<div class="card tournament-card">
					<div class="tournament-header">
						<h3>{t.name}</h3>
						<span class="badge" class:badge-success={t.status === 'active'}>{t.status}</span>
					</div>
					<p>{t.description}</p>
					<div class="tournament-meta">
						<span>Scoring: {t.scoring}</span>
						<span>Max ticks: {t.max_ticks}</span>
					</div>

					{#if t.leaderboard.length > 0}
						<div class="leaderboard">
							<h4>Leaderboard</h4>
							<ol>
								{#each t.leaderboard as entry, i}
									<li class="leaderboard-entry">
										<span class="rank">#{i + 1}</span>
										<span class="bot-name">{entry.bot_name}</span>
										<span class="score">{entry.score}</span>
									</li>
								{/each}
							</ol>
						</div>
					{:else}
						<p class="no-results">No results yet.</p>
					{/if}
				</div>
			{/each}
		</div>
	{/if}
</div>

<style>
	.tournament-grid {
		display: flex;
		flex-direction: column;
		gap: var(--sp-4);
	}

	.tournament-card h3 {
		margin-bottom: 0;
	}

	.tournament-header {
		display: flex;
		justify-content: space-between;
		align-items: center;
		margin-bottom: var(--sp-3);
	}

	.tournament-meta {
		display: flex;
		gap: var(--sp-4);
		font-size: var(--text-sm);
		color: var(--text-muted);
		margin-bottom: var(--sp-4);
	}

	.leaderboard h4 {
		font-size: var(--text-sm);
		text-transform: uppercase;
		letter-spacing: 0.05em;
		color: var(--text-muted);
		border-top: 1px solid var(--border);
		padding-top: var(--sp-4);
	}

	.leaderboard ol {
		list-style: none;
		padding: 0;
		margin: 0;
	}

	.leaderboard-entry {
		display: flex;
		align-items: center;
		gap: var(--sp-3);
		padding: var(--sp-2) 0;
		border-bottom: 1px solid var(--border);
		font-size: var(--text-sm);
		margin-bottom: 0;
	}

	.leaderboard-entry:last-child {
		border-bottom: none;
	}

	.rank {
		font-weight: 700;
		font-family: var(--font-mono);
		color: var(--accent);
		min-width: 2rem;
	}

	.bot-name {
		flex: 1;
		font-weight: 500;
	}

	.score {
		font-family: var(--font-mono);
		font-weight: 600;
		color: var(--text-secondary);
	}

	.no-results {
		font-size: var(--text-sm);
		color: var(--text-muted);
		margin-bottom: 0;
	}

	.empty-state {
		text-align: center;
		padding: var(--sp-8);
	}
</style>
