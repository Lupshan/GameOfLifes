<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { renderWorld, renderHUD } from './renderer';
	import type { Camera, Snapshot } from './types';

	let { snapshot }: { snapshot: Snapshot | null } = $props();

	let canvas: HTMLCanvasElement;
	let ctx: CanvasRenderingContext2D | null = null;
	let animFrameId: number = 0;

	let camera: Camera = { x: 0, y: 0, zoom: 8 };

	let dragging = false;
	let dragStartX = 0;
	let dragStartY = 0;
	let camStartX = 0;
	let camStartY = 0;

	function renderLoop() {
		if (!ctx || !canvas) return;

		const w = canvas.clientWidth;
		const h = canvas.clientHeight;
		if (canvas.width !== w || canvas.height !== h) {
			canvas.width = w;
			canvas.height = h;
		}

		if (snapshot) {
			renderWorld(ctx, snapshot, camera, canvas.width, canvas.height);
			renderHUD(ctx, snapshot, canvas.width);
		} else {
			ctx.clearRect(0, 0, canvas.width, canvas.height);
			ctx.fillStyle = '#333';
			ctx.fillRect(0, 0, canvas.width, canvas.height);
			ctx.fillStyle = '#aaa';
			ctx.font = '20px monospace';
			ctx.fillText('No data', 20, 40);
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
		camera = { ...camera, zoom: Math.max(2, Math.min(40, camera.zoom * factor)) };
	}

	onMount(() => {
		ctx = canvas.getContext('2d');
		animFrameId = requestAnimationFrame(renderLoop);
	});

	onDestroy(() => {
		if (animFrameId) cancelAnimationFrame(animFrameId);
	});
</script>

<canvas
	bind:this={canvas}
	style="width:100%;height:100%;cursor:grab;"
	onmousedown={onMouseDown}
	onmousemove={onMouseMove}
	onmouseup={onMouseUp}
	onmouseleave={onMouseUp}
	onwheel={onWheel}
></canvas>
