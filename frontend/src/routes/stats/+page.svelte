<script lang="ts">
	import { onMount } from 'svelte';
	import { apiFetch } from '$lib/api';
	import LineChart from '$lib/charts/LineChart.svelte';

	interface TickBucket {
		tick_bucket: number;
		births: number;
		deaths: number;
	}

	let buckets = $state<TickBucket[]>([]);
	let totalBirths = $state(0);
	let totalDeaths = $state(0);
	let loading = $state(true);

	onMount(async () => {
		const resp = await apiFetch('/stats?from=0&to=999999999');
		if (resp.ok) {
			const data = await resp.json();
			buckets = data.buckets;
			totalBirths = data.total_births;
			totalDeaths = data.total_deaths;
		}
		loading = false;
	});

	let birthSeries = $derived({
		label: 'Births',
		color: '#10b981',
		data: buckets.map((b) => ({ x: b.tick_bucket, y: b.births }))
	});

	let deathSeries = $derived({
		label: 'Deaths',
		color: '#ef4444',
		data: buckets.map((b) => ({ x: b.tick_bucket, y: b.deaths }))
	});
</script>

<svelte:head>
	<title>Stats - GameOfLifes</title>
</svelte:head>

<div class="container">
	<h1>Simulation Stats</h1>

	{#if loading}
		<p class="loading">Loading statistics...</p>
	{:else if buckets.length === 0}
		<div class="card empty-state">
			<p>No data yet. Run the simulation to see stats.</p>
			<a href="/world" class="btn btn-primary">Watch the World</a>
		</div>
	{:else}
		<div class="summary-cards">
			<div class="summary-card card">
				<span class="summary-label">Total Births</span>
				<span class="summary-value births">{totalBirths.toLocaleString()}</span>
			</div>
			<div class="summary-card card">
				<span class="summary-label">Total Deaths</span>
				<span class="summary-value deaths">{totalDeaths.toLocaleString()}</span>
			</div>
			<div class="summary-card card">
				<span class="summary-label">Net Growth</span>
				<span
					class="summary-value"
					class:births={totalBirths - totalDeaths > 0}
					class:deaths={totalBirths - totalDeaths < 0}
				>
					{(totalBirths - totalDeaths).toLocaleString()}
				</span>
			</div>
		</div>

		<div class="card chart-card">
			<h3>Births & Deaths over Time</h3>
			<div class="chart-wrapper">
				<LineChart series={[birthSeries, deathSeries]} width={700} height={300} />
			</div>
		</div>
	{/if}
</div>

<style>
	.summary-cards {
		display: grid;
		grid-template-columns: repeat(3, 1fr);
		gap: var(--sp-4);
		margin-bottom: var(--sp-6);
	}

	.summary-card {
		text-align: center;
		padding: var(--sp-5);
	}

	.summary-label {
		display: block;
		font-size: var(--text-sm);
		color: var(--text-muted);
		text-transform: uppercase;
		letter-spacing: 0.05em;
		margin-bottom: var(--sp-2);
	}

	.summary-value {
		display: block;
		font-size: var(--text-2xl);
		font-weight: 700;
		color: var(--text-primary);
	}

	.summary-value.births {
		color: var(--accent);
	}

	.summary-value.deaths {
		color: var(--danger);
	}

	.chart-card h3 {
		margin-bottom: var(--sp-4);
	}

	.chart-wrapper {
		overflow-x: auto;
	}

	.empty-state {
		text-align: center;
		padding: var(--sp-12);
	}

	.empty-state p {
		margin-bottom: var(--sp-4);
	}

	@media (max-width: 768px) {
		.summary-cards {
			grid-template-columns: 1fr;
		}
	}
</style>
