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
		if (!$auth.token) {
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

<div style="max-width:800px;margin:2rem auto;padding:1rem;">
	<h1>Sandbox — Private Simulations</h1>

	<div style="margin-bottom:1rem;display:flex;gap:0.5rem;">
		<input type="text" bind:value={simName} placeholder="Simulation name"
			style="flex:1;padding:0.5rem;" />
		<button onclick={createSim} disabled={creating || !simName.trim()}
			style="padding:0.5rem 1rem;cursor:pointer;">
			{creating ? 'Creating...' : 'New Sim'}
		</button>
	</div>

	{#if loading}
		<p>Loading...</p>
	{:else if sims.length === 0}
		<p>No private simulations. Create one above.</p>
	{:else}
		<table style="width:100%;border-collapse:collapse;">
			<thead>
				<tr style="text-align:left;border-bottom:2px solid #ccc;">
					<th style="padding:0.5rem;">Name</th>
					<th style="padding:0.5rem;">Status</th>
					<th style="padding:0.5rem;">Max Ticks</th>
					<th style="padding:0.5rem;">Actions</th>
				</tr>
			</thead>
			<tbody>
				{#each sims as sim}
					<tr style="border-bottom:1px solid #eee;">
						<td style="padding:0.5rem;">{sim.name}</td>
						<td style="padding:0.5rem;">{sim.status}</td>
						<td style="padding:0.5rem;">{sim.max_ticks}</td>
						<td style="padding:0.5rem;">
							<button onclick={() => deleteSim(sim.id)}
								style="cursor:pointer;color:red;background:none;border:none;">Delete</button>
						</td>
					</tr>
				{/each}
			</tbody>
		</table>
	{/if}
</div>
