<script lang="ts">
	import { onMount } from 'svelte';
	import { apiFetch } from '$lib/api';
	import { auth } from '$lib/stores/auth';
	import { goto } from '$app/navigation';

	interface Sim {
		id: string;
		name: string;
		status: string;
		max_ticks: number;
		created_at: string;
	}

	let sims = $state<Sim[]>([]);
	let loading = $state(true);
	let simName = $state('');
	let creating = $state(false);

	onMount(async () => {
		if (!$auth.loggedIn) {
			goto('/login');
			return;
		}
		await loadSims();
	});

	async function loadSims() {
		loading = true;
		const resp = await apiFetch('/sims');
		if (resp.ok) {
			sims = await resp.json();
		}
		loading = false;
	}

	async function createSim() {
		if (!simName.trim()) return;
		creating = true;
		const resp = await apiFetch('/sims', {
			method: 'POST',
			body: JSON.stringify({ name: simName, config: { max_ticks: 1000 }, bot_ids: [] })
		});
		if (resp.ok) {
			simName = '';
			await loadSims();
		}
		creating = false;
	}

	async function deleteSim(id: string) {
		await apiFetch(`/sims/${id}`, { method: 'DELETE' });
		await loadSims();
	}
</script>

<svelte:head>
	<title>Sandbox - GameOfLifes</title>
</svelte:head>

<div class="container">
	<h1>Sandbox</h1>
	<p>Private simulations for testing your bots in isolation.</p>

	<div class="create-bar card">
		<input type="text" bind:value={simName} placeholder="Simulation name" />
		<button class="btn-primary" onclick={createSim} disabled={creating || !simName.trim()}>
			{creating ? 'Creating...' : 'New Simulation'}
		</button>
	</div>

	{#if loading}
		<p class="loading">Loading simulations...</p>
	{:else if sims.length === 0}
		<div class="card empty-state">
			<p>No private simulations. Create one above to start testing.</p>
		</div>
	{:else}
		<div class="card">
			<table>
				<thead>
					<tr>
						<th>Name</th>
						<th>Status</th>
						<th>Max Ticks</th>
						<th>Actions</th>
					</tr>
				</thead>
				<tbody>
					{#each sims as sim}
						<tr>
							<td><strong>{sim.name}</strong></td>
							<td><span class="badge">{sim.status}</span></td>
							<td>{sim.max_ticks}</td>
							<td>
								<button class="btn-danger btn-sm" onclick={() => deleteSim(sim.id)}>Delete</button>
							</td>
						</tr>
					{/each}
				</tbody>
			</table>
		</div>
	{/if}
</div>

<style>
	.create-bar {
		display: flex;
		gap: var(--sp-3);
		align-items: center;
		margin-bottom: var(--sp-6);
	}

	.create-bar input {
		flex: 1;
	}

	.btn-sm {
		padding: var(--sp-1) var(--sp-3);
		font-size: var(--text-xs);
	}

	.empty-state {
		text-align: center;
		padding: var(--sp-8);
	}
</style>
