<script lang="ts">
	import { onMount } from 'svelte';
	import { page } from '$app/state';
	import { apiFetch } from '$lib/api';

	interface LineageEvent {
		id: string;
		tick: number;
		event_type: string;
		agent_id: number;
		parent_agent_id: number | null;
		generation: number | null;
	}

	let events = $state<LineageEvent[]>([]);
	let loading = $state(true);

	onMount(async () => {
		const resp = await apiFetch(`/lineage?bot_id=${page.params.bot_id}&from_tick=0&to_tick=999999`);
		if (resp.ok) {
			events = await resp.json();
		}
		loading = false;
	});

	// Build tree: parent → children.
	let births = $derived(events.filter((e) => e.event_type === 'birth'));
	let tree = $derived(() => {
		const map = new Map<number, LineageEvent[]>();
		for (const b of births) {
			if (b.parent_agent_id != null) {
				const children = map.get(b.parent_agent_id) ?? [];
				children.push(b);
				map.set(b.parent_agent_id, children);
			}
		}
		return map;
	});

	// Find root agents (no parent in our data).
	let roots = $derived(
		births.filter((b) => b.parent_agent_id == null || !births.some((p) => p.agent_id === b.parent_agent_id))
			.slice(0, 20)
	);
</script>

<svelte:head>
	<title>Genealogy - GameOfLifes</title>
</svelte:head>

<div style="max-width:800px;margin:2rem auto;padding:1rem;">
	<h1>Genealogy</h1>
	<p>Bot: {page.params.bot_id}</p>

	{#if loading}
		<p>Loading...</p>
	{:else if births.length === 0}
		<p>No lineage data yet.</p>
	{:else}
		<p>{births.length} birth events found.</p>

		<div style="font-family:monospace;font-size:13px;">
			{#each roots as root}
				<div style="margin-bottom:0.5rem;">
					<span>Agent #{root.agent_id} (gen {root.generation ?? 0}, tick {root.tick})</span>
					{#each tree().get(root.agent_id) ?? [] as child}
						<div style="margin-left:2rem;">
							└─ Agent #{child.agent_id} (gen {child.generation ?? 0}, tick {child.tick})
						</div>
					{/each}
				</div>
			{/each}
		</div>
	{/if}

	<p style="margin-top:2rem;"><a href="/stats">← Stats</a></p>
</div>
