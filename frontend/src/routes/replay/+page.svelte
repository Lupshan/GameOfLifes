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

	// Cache of fetched snapshots.
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

		// Prefetch next 10.
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

<div style="display:flex;flex-direction:column;height:calc(100vh - 50px);">
	<div style="flex:1;position:relative;">
		<WorldCanvas snapshot={currentSnapshot} />
	</div>

	<div style="padding:0.5rem 1rem;background:#222;color:#ddd;display:flex;align-items:center;gap:1rem;">
		{#if loading}
			<span>Loading snapshots...</span>
		{:else if snapshots.length === 0}
			<span>No snapshots available.</span>
		{:else}
			<button onclick={togglePlay}
				style="padding:0.25rem 0.75rem;cursor:pointer;">
				{playing ? '⏸' : '▶'}
			</button>

			<input
				type="range"
				min="0"
				max={snapshots.length - 1}
				value={currentIndex}
				oninput={onScrub}
				style="flex:1;"
			/>

			<span style="font-size:13px;min-width:120px;">
				Tick {snapshots[currentIndex]?.tick ?? '—'} ({currentIndex + 1}/{snapshots.length})
			</span>

			<div style="display:flex;gap:0.25rem;">
				{#each [1, 2, 4, 8] as s}
					<button
						onclick={() => setSpeed(s)}
						style="padding:0.2rem 0.5rem;cursor:pointer;background:{speed === s ? '#4caf50' : '#444'};color:#fff;border:none;">
						{s}x
					</button>
				{/each}
			</div>
		{/if}
	</div>
</div>
