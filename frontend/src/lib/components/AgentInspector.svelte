<script lang="ts">
	import type { SnapshotAgent } from '$lib/canvas/types';

	let { agent, onclose }: { agent: SnapshotAgent; onclose: () => void } = $props();

	function traitFromGenome(genome: string, byteIndex: number, min: number, max: number): number {
		if (genome.length < (byteIndex + 1) * 2) return min;
		const byte = parseInt(genome.slice(byteIndex * 2, byteIndex * 2 + 2), 16);
		return min + Math.round((byte / 255) * (max - min));
	}
</script>

<div style="position:fixed;top:50px;right:0;width:300px;height:calc(100vh - 50px);background:#222;color:#ddd;padding:1rem;overflow-y:auto;border-left:2px solid #444;">
	<div style="display:flex;justify-content:space-between;align-items:center;">
		<h3 style="margin:0;">Agent #{agent.id}</h3>
		<button onclick={onclose} style="background:none;border:none;color:#aaa;cursor:pointer;font-size:18px;">✕</button>
	</div>

	<table style="width:100%;margin-top:1rem;font-size:13px;">
		<tr><td>Position</td><td>({agent.x}, {agent.y})</td></tr>
		<tr><td>Energy</td><td>{agent.energy}</td></tr>
		<tr><td>Hydration</td><td>{agent.hydration}</td></tr>
		<tr><td>Generation</td><td>{agent.generation}</td></tr>
		<tr><td>Parent</td><td>{agent.parent_id === 18446744073709551615 ? '—' : `#${agent.parent_id}`}</td></tr>
		<tr><td>Genome</td><td style="word-break:break-all;font-family:monospace;">{agent.genome}</td></tr>
	</table>

	<h4 style="margin-top:1rem;">Traits</h4>
	<table style="width:100%;font-size:13px;">
		<tr><td>Vision</td><td>{traitFromGenome(agent.genome, 0, 1, 5)}</td></tr>
		<tr><td>Speed</td><td>{traitFromGenome(agent.genome, 1, 1, 3)}</td></tr>
		<tr><td>Metabolism</td><td>{traitFromGenome(agent.genome, 2, 1, 5)}</td></tr>
		<tr><td>Repro threshold</td><td>{traitFromGenome(agent.genome, 3, 50, 200)}</td></tr>
		<tr><td>Swim</td><td>{traitFromGenome(agent.genome, 4, 0, 3)}</td></tr>
		<tr><td>Forage</td><td>{traitFromGenome(agent.genome, 5, 1, 3)}</td></tr>
	</table>
</div>
