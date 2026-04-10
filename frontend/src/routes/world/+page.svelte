<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { renderWorld, renderHUD } from '$lib/canvas/renderer';
	import type { Camera, Snapshot } from '$lib/canvas/types';
	import { isValidSnapshot } from '$lib/canvas/validate';

	let canvas: HTMLCanvasElement;
	let ctx: CanvasRenderingContext2D | null = null;
	let ws: WebSocket | null = null;
	let latestSnapshot: Snapshot | null = null;
	let animFrameId: number = 0;

	let camera: Camera = { x: 0, y: 0, zoom: 8 };

	let retryDelay = 1000;
	let retryCount = 0;

	let dragging = false;
	let dragStartX = 0;
	let dragStartY = 0;
	let camStartX = 0;
	let camStartY = 0;

	function connectWS() {
		const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
		const wsUrl = `${protocol}//${window.location.host}/ws/world`;
		ws = new WebSocket(wsUrl);

		ws.onopen = () => {
			retryDelay = 1000;
		};

		ws.onmessage = (event) => {
			try {
				const parsed = JSON.parse(event.data);
				if (isValidSnapshot(parsed)) {
					latestSnapshot = parsed;
				}
			} catch {
				// Ignore parse errors.
			}
		};

		ws.onclose = () => {
			if (retryCount < 10) {
				const jitter = Math.random() * 500;
				setTimeout(connectWS, retryDelay + jitter);
				retryDelay = Math.min(retryDelay * 2, 30000);
				retryCount++;
			}
		};
	}

	function renderLoop() {
		if (!ctx || !canvas) return;

		const w = window.innerWidth;
		const h = window.innerHeight;
		if (canvas.width !== w || canvas.height !== h) {
			canvas.width = w;
			canvas.height = h;
		}

		if (latestSnapshot) {
			renderWorld(ctx, latestSnapshot, camera, canvas.width, canvas.height);
			renderHUD(ctx, latestSnapshot, canvas.width);
		} else {
			ctx.clearRect(0, 0, canvas.width, canvas.height);
			ctx.fillStyle = '#0b0f19';
			ctx.fillRect(0, 0, canvas.width, canvas.height);
			ctx.fillStyle = '#64748b';
			ctx.font = '16px Inter, system-ui, sans-serif';
			ctx.fillText('Connecting to world...', 20, 40);
		}

		animFrameId = requestAnimationFrame(renderLoop);
	}

	function onMouseDown(e: MouseEvent) {
		dragging = true;
		dragStartX = e.clientX;
		dragStartY = e.clientY;
		camStartX = camera.x;
		camStartY = camera.y;
	}

	function onMouseMove(e: MouseEvent) {
		if (!dragging) return;
		const dx = (e.clientX - dragStartX) / camera.zoom;
		const dy = (e.clientY - dragStartY) / camera.zoom;
		camera = { ...camera, x: camStartX - dx, y: camStartY - dy };
	}

	function onMouseUp() {
		dragging = false;
	}

	function onWheel(e: WheelEvent) {
		e.preventDefault();
		const factor = e.deltaY > 0 ? 0.9 : 1.1;
		const newZoom = Math.max(2, Math.min(40, camera.zoom * factor));
		camera = { ...camera, zoom: newZoom };
	}

	onMount(() => {
		ctx = canvas.getContext('2d');
		connectWS();
		animFrameId = requestAnimationFrame(renderLoop);
	});

	onDestroy(() => {
		if (ws) ws.close();
		if (animFrameId) cancelAnimationFrame(animFrameId);
	});
</script>

<svelte:head>
	<title>World - GameOfLifes</title>
</svelte:head>

<canvas
	bind:this={canvas}
	class="world-canvas"
	onmousedown={onMouseDown}
	onmousemove={onMouseMove}
	onmouseup={onMouseUp}
	onmouseleave={onMouseUp}
	onwheel={onWheel}
></canvas>

<style>
	.world-canvas {
		position: fixed;
		top: 0;
		left: 0;
		width: 100vw;
		height: 100vh;
		cursor: grab;
		z-index: 50;
	}

	.world-canvas:active {
		cursor: grabbing;
	}
</style>
