<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { apiFetch } from '$lib/api';
	import WorldCanvas from '$lib/canvas/WorldCanvas.svelte';
	import type { Snapshot } from '$lib/canvas/types';

	interface SnapshotMeta {
		id: string;
		tick: number;
	}

	let snapshots = $state<SnapshotMeta[]>([]);
	let currentIndex = $state(0);
	let currentSnapshot = $state<Snapshot | null>(null);
	let playing = $state(false);
	let speed = $state(1);
	let loading = $state(true);
	let intervalId: ReturnType<typeof setInterval> | null = null;

	const cache = new Map<string, Snapshot>();

	onMount(async () => {
		const resp = await apiFetch('/snapshots?from=0&to=999999999');
		if (resp.ok) {
			snapshots = await resp.json();
		}
		loading = false;

		if (snapshots.length > 0) {
			await loadSnapshot(0);
		}
	});

	onDestroy(() => {
		stopPlayback();
	});

	async function loadSnapshot(index: number) {
		if (index < 0 || index >= snapshots.length) return;
		currentIndex = index;
		const meta = snapshots[index];

		if (cache.has(meta.id)) {
			currentSnapshot = cache.get(meta.id)!;
		} else {
			const resp = await apiFetch(`/snapshots/${meta.id}`);
			if (resp.ok) {
				const data = await resp.json();
				cache.set(meta.id, data);
				currentSnapshot = data;
			}
		}

		for (let i = index + 1; i < Math.min(index + 11, snapshots.length); i++) {
			const m = snapshots[i];
			if (!cache.has(m.id)) {
				apiFetch(`/snapshots/${m.id}`).then(async (r) => {
					if (r.ok) {
						cache.set(m.id, await r.json());
					}
				});
			}
		}
	}

	function togglePlay() {
		if (playing) {
			stopPlayback();
		} else {
			playing = true;
			intervalId = setInterval(() => {
				if (currentIndex < snapshots.length - 1) {
					loadSnapshot(currentIndex + 1);
				} else {
					stopPlayback();
				}
			}, 1000 / speed);
		}
	}

	function stopPlayback() {
		playing = false;
		if (intervalId !== null) {
			clearInterval(intervalId);
			intervalId = null;
		}
	}

	function setSpeed(s: number) {
		speed = s;
		if (playing) {
			stopPlayback();
			togglePlay();
		}
	}

	function onScrub(e: Event) {
		const target = e.target as HTMLInputElement;
		const idx = parseInt(target.value, 10);
		stopPlayback();
		loadSnapshot(idx);
	}
</script>

<svelte:head>
	<title>Replay - GameOfLifes</title>
</svelte:head>

<div class="replay-layout">
	<div class="canvas-area">
		<WorldCanvas snapshot={currentSnapshot} />
	</div>

	<div class="controls">
		{#if loading}
			<span class="controls-text">Loading snapshots...</span>
		{:else if snapshots.length === 0}
			<span class="controls-text">No snapshots available.</span>
		{:else}
			<button class="play-btn" onclick={togglePlay}>
				{playing ? '\u23F8' : '\u25B6'}
			</button>

			<input
				type="range"
				min="0"
				max={snapshots.length - 1}
				value={currentIndex}
				oninput={onScrub}
				class="scrubber"
			/>

			<span class="tick-info">
				Tick {snapshots[currentIndex]?.tick ?? '\u2014'} ({currentIndex + 1}/{snapshots.length})
			</span>

			<div class="speed-group">
				{#each [1, 2, 4, 8] as s}
					<button class="speed-btn" class:speed-active={speed === s} onclick={() => setSpeed(s)}>
						{s}x
					</button>
				{/each}
			</div>
		{/if}
	</div>
</div>

<style>
	.replay-layout {
		display: flex;
		flex-direction: column;
		height: calc(100vh - var(--nav-height));
	}

	.canvas-area {
		flex: 1;
		position: relative;
		background: var(--bg-base);
	}

	.controls {
		display: flex;
		align-items: center;
		gap: var(--sp-3);
		padding: var(--sp-3) var(--sp-4);
		background: var(--bg-surface);
		border-top: 1px solid var(--border);
	}

	.controls-text {
		font-size: var(--text-sm);
		color: var(--text-muted);
	}

	.play-btn {
		width: 40px;
		height: 40px;
		border-radius: 50%;
		font-size: var(--text-lg);
		display: flex;
		align-items: center;
		justify-content: center;
		flex-shrink: 0;
		background: var(--accent);
		border-color: var(--accent);
		color: #fff;
	}

	.play-btn:hover {
		background: var(--accent-hover);
		border-color: var(--accent-hover);
	}

	.scrubber {
		flex: 1;
	}

	.tick-info {
		font-size: var(--text-xs);
		font-family: var(--font-mono);
		color: var(--text-secondary);
		min-width: 140px;
		text-align: right;
		flex-shrink: 0;
	}

	.speed-group {
		display: flex;
		gap: var(--sp-1);
		flex-shrink: 0;
	}

	.speed-btn {
		padding: var(--sp-1) var(--sp-2);
		font-size: var(--text-xs);
		font-weight: 600;
		border-radius: var(--radius-sm);
	}

	.speed-active {
		background: var(--accent);
		border-color: var(--accent);
		color: #fff;
	}

	@media (max-width: 768px) {
		.tick-info {
			display: none;
		}
	}
</style>
