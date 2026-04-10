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
		color: '#4caf50',
		data: buckets.map((b) => ({ x: b.tick_bucket, y: b.births }))
	});

	let deathSeries = $derived({
		label: 'Deaths',
		color: '#f44336',
		data: buckets.map((b) => ({ x: b.tick_bucket, y: b.deaths }))
	});
</script>

<svelte:head>
	<title>Stats - GameOfLifes</title>
</svelte:head>

<div style="max-width:800px;margin:2rem auto;padding:1rem;">
	<h1>Simulation Stats</h1>

	{#if loading}
		<p>Loading...</p>
	{:else if buckets.length === 0}
		<p>No data yet. Run the simulation to see stats.</p>
	{:else}
		<p>Total births: {totalBirths} | Total deaths: {totalDeaths}</p>

		<h3>Births & Deaths over Time</h3>
		<LineChart series={[birthSeries, deathSeries]} width={700} height={300} />
	{/if}

	<p style="margin-top:2rem;"><a href="/world">← Back to World</a></p>
</div>
