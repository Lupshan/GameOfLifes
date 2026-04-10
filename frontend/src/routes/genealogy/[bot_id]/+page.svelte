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

	let roots = $derived(
		births
			.filter(
				(b) => b.parent_agent_id == null || !births.some((p) => p.agent_id === b.parent_agent_id)
			)
			.slice(0, 20)
	);
</script>

<svelte:head>
	<title>Genealogy - GameOfLifes</title>
</svelte:head>

<div class="container">
	<h1>Genealogy</h1>
	<p class="bot-id">Bot: <code>{page.params.bot_id}</code></p>

	{#if loading}
		<p class="loading">Loading lineage data...</p>
	{:else if births.length === 0}
		<div class="card empty-state">
			<p>No lineage data yet for this bot.</p>
		</div>
	{:else}
		<p class="event-count">{births.length} birth events found</p>

		<div class="card tree-card">
			{#each roots as root}
				<div class="tree-node">
					<div class="agent-line">
						<span class="agent-id">Agent #{root.agent_id}</span>
						<span class="badge">Gen {root.generation ?? 0}</span>
						<span class="tick-label">tick {root.tick}</span>
					</div>
					{#each tree().get(root.agent_id) ?? [] as child}
						<div class="child-node">
							<span class="tree-branch"></span>
							<span class="agent-id">Agent #{child.agent_id}</span>
							<span class="badge">Gen {child.generation ?? 0}</span>
							<span class="tick-label">tick {child.tick}</span>
						</div>
					{/each}
				</div>
			{/each}
		</div>
	{/if}

	<a href="/stats" class="back-link">&larr; Stats</a>
</div>

<style>
	.bot-id {
		color: var(--text-muted);
		margin-bottom: var(--sp-4);
	}

	.event-count {
		font-size: var(--text-sm);
		color: var(--text-secondary);
		margin-bottom: var(--sp-4);
	}

	.tree-card {
		font-family: var(--font-mono);
		font-size: var(--text-sm);
	}

	.tree-node {
		padding: var(--sp-3) 0;
		border-bottom: 1px solid var(--border);
	}

	.tree-node:last-child {
		border-bottom: none;
	}

	.agent-line {
		display: flex;
		align-items: center;
		gap: var(--sp-2);
	}

	.agent-id {
		font-weight: 600;
		color: var(--text-primary);
	}

	.tick-label {
		font-size: var(--text-xs);
		color: var(--text-muted);
		margin-left: auto;
	}

	.child-node {
		display: flex;
		align-items: center;
		gap: var(--sp-2);
		padding: var(--sp-1) 0;
		margin-left: var(--sp-6);
	}

	.tree-branch {
		color: var(--text-muted);
	}

	.tree-branch::before {
		content: '\2514\2500';
	}

	.empty-state {
		text-align: center;
		padding: var(--sp-12);
	}

	.back-link {
		display: inline-block;
		margin-top: var(--sp-6);
		font-size: var(--text-sm);
	}
</style>
