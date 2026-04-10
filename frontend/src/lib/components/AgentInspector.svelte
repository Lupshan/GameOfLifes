<script lang="ts">
	import type { SnapshotAgent } from '$lib/canvas/types';
	import { NO_PARENT } from '$lib/types/api';

	let { agent, onclose }: { agent: SnapshotAgent; onclose: () => void } = $props();

	function traitFromGenome(genome: string, byteIndex: number, min: number, max: number): number {
		if (genome.length < (byteIndex + 1) * 2) return min;
		const byte = parseInt(genome.slice(byteIndex * 2, byteIndex * 2 + 2), 16);
		return min + Math.round((byte / 255) * (max - min));
	}
</script>

<div class="inspector">
	<div class="inspector-header">
		<h3>Agent #{agent.id}</h3>
		<button class="close-btn" onclick={onclose}>&times;</button>
	</div>

	<table class="props-table">
		<tbody>
			<tr><td>Position</td><td>({agent.x}, {agent.y})</td></tr>
			<tr><td>Energy</td><td>{agent.energy}</td></tr>
			<tr><td>Hydration</td><td>{agent.hydration}</td></tr>
			<tr><td>Generation</td><td>{agent.generation}</td></tr>
			<tr
				><td>Parent</td><td
					>{BigInt(agent.parent_id) === NO_PARENT ? '\u2014' : `#${agent.parent_id}`}</td
				></tr
			>
			<tr><td>Genome</td><td class="genome-cell">{agent.genome}</td></tr>
		</tbody>
	</table>

	<h4>Traits</h4>
	<table class="props-table">
		<tbody>
			<tr><td>Vision</td><td>{traitFromGenome(agent.genome, 0, 1, 5)}</td></tr>
			<tr><td>Speed</td><td>{traitFromGenome(agent.genome, 1, 1, 3)}</td></tr>
			<tr><td>Metabolism</td><td>{traitFromGenome(agent.genome, 2, 1, 5)}</td></tr>
			<tr><td>Repro threshold</td><td>{traitFromGenome(agent.genome, 3, 50, 200)}</td></tr>
			<tr><td>Swim</td><td>{traitFromGenome(agent.genome, 4, 0, 3)}</td></tr>
			<tr><td>Forage</td><td>{traitFromGenome(agent.genome, 5, 1, 3)}</td></tr>
		</tbody>
	</table>
</div>

<style>
	.inspector {
		position: fixed;
		top: var(--nav-height);
		right: 0;
		width: 300px;
		height: calc(100vh - var(--nav-height));
		background: var(--bg-surface);
		color: var(--text-primary);
		padding: var(--sp-4);
		overflow-y: auto;
		border-left: 1px solid var(--border);
		z-index: 60;
	}

	.inspector-header {
		display: flex;
		justify-content: space-between;
		align-items: center;
		margin-bottom: var(--sp-4);
	}

	.inspector-header h3 {
		margin: 0;
		font-size: var(--text-base);
	}

	.close-btn {
		background: none;
		border: none;
		color: var(--text-muted);
		font-size: 1.25rem;
		cursor: pointer;
		padding: var(--sp-1);
		line-height: 1;
	}

	.close-btn:hover {
		color: var(--text-primary);
		background: transparent;
	}

	h4 {
		margin-top: var(--sp-4);
		font-size: var(--text-sm);
		text-transform: uppercase;
		letter-spacing: 0.05em;
		color: var(--text-muted);
	}

	.props-table {
		font-size: var(--text-sm);
	}

	.props-table td:first-child {
		color: var(--text-secondary);
		padding-right: var(--sp-4);
	}

	.genome-cell {
		word-break: break-all;
		font-family: var(--font-mono);
		font-size: var(--text-xs);
	}
</style>
