/** Canvas2D world renderer. */

import type { Camera, Snapshot, SnapshotAgent } from './types';
import { decodeBitGrid, decodeTerrainGrid } from './decode';

// Terrain colors.
const TERRAIN_COLORS = ['#d4c89a', '#2d6a1e', '#1a6b8a', '#7a7a7a']; // Plain, Forest, Water, Rock

// Resource colors.
const FOOD_COLOR = '#f5c542';
const WATER_COLOR = '#42b0f5';
const MINERAL_COLOR = '#f5a442';

/** Get agent color from genome hex (first 3 bytes → RGB). */
function agentColor(genome: string): string {
	if (genome.length < 6) return '#ffffff';
	const r = parseInt(genome.slice(0, 2), 16);
	const g = parseInt(genome.slice(2, 4), 16);
	const b = parseInt(genome.slice(4, 6), 16);
	return `rgb(${r},${g},${b})`;
}

/** Render the world onto a canvas context. */
export function renderWorld(
	ctx: CanvasRenderingContext2D,
	snapshot: Snapshot,
	camera: Camera,
	canvasWidth: number,
	canvasHeight: number
): void {
	const { width, height } = snapshot;
	const cellSize = camera.zoom;

	ctx.clearRect(0, 0, canvasWidth, canvasHeight);
	ctx.save();
	ctx.translate(-camera.x * cellSize, -camera.y * cellSize);

	const totalCells = width * height;

	// Decode grids.
	const terrain = decodeTerrainGrid(snapshot.terrain);
	const food = decodeBitGrid(snapshot.food, totalCells);
	const water = decodeBitGrid(snapshot.water, totalCells);
	const mineral = decodeBitGrid(snapshot.mineral, totalCells);

	// Compute visible range.
	const startX = Math.max(0, Math.floor(camera.x));
	const startY = Math.max(0, Math.floor(camera.y));
	const endX = Math.min(width, Math.ceil(camera.x + canvasWidth / cellSize));
	const endY = Math.min(height, Math.ceil(camera.y + canvasHeight / cellSize));

	// Draw terrain.
	for (let y = startY; y < endY; y++) {
		for (let x = startX; x < endX; x++) {
			const idx = y * width + x;
			const t = terrain[idx] ?? 0;
			ctx.fillStyle = TERRAIN_COLORS[t] ?? TERRAIN_COLORS[0];
			ctx.fillRect(x * cellSize, y * cellSize, cellSize, cellSize);
		}
	}

	// Draw resources (small dots).
	const dotSize = Math.max(2, cellSize * 0.3);
	for (let y = startY; y < endY; y++) {
		for (let x = startX; x < endX; x++) {
			const idx = y * width + x;
			const cx = x * cellSize + cellSize / 2;
			const cy = y * cellSize + cellSize / 2;

			if (food[idx]) {
				ctx.fillStyle = FOOD_COLOR;
				ctx.fillRect(cx - dotSize / 2, cy - dotSize / 2, dotSize, dotSize);
			}
			if (water[idx]) {
				ctx.fillStyle = WATER_COLOR;
				ctx.fillRect(cx - dotSize / 2 + dotSize, cy - dotSize / 2, dotSize, dotSize);
			}
			if (mineral[idx]) {
				ctx.fillStyle = MINERAL_COLOR;
				ctx.fillRect(cx - dotSize / 2, cy - dotSize / 2 + dotSize, dotSize, dotSize);
			}
		}
	}

	// Draw agents.
	const agentRadius = Math.max(2, cellSize * 0.35);
	for (const agent of snapshot.agents) {
		if (!agent.alive) continue;
		if (agent.x < startX || agent.x >= endX || agent.y < startY || agent.y >= endY) continue;

		const cx = agent.x * cellSize + cellSize / 2;
		const cy = agent.y * cellSize + cellSize / 2;

		ctx.fillStyle = agentColor(agent.genome);
		ctx.beginPath();
		ctx.arc(cx, cy, agentRadius, 0, Math.PI * 2);
		ctx.fill();
	}

	ctx.restore();
}

/** Render HUD overlay. */
export function renderHUD(
	ctx: CanvasRenderingContext2D,
	snapshot: Snapshot,
	canvasWidth: number
): void {
	const aliveAgents = snapshot.agents.filter((a) => a.alive).length;

	ctx.save();
	ctx.fillStyle = 'rgba(0, 0, 0, 0.6)';
	ctx.fillRect(canvasWidth - 220, 10, 210, 80);

	ctx.fillStyle = '#ffffff';
	ctx.font = '14px monospace';
	ctx.fillText(`Tick: ${snapshot.tick}`, canvasWidth - 210, 30);
	ctx.fillText(`Agents: ${aliveAgents}`, canvasWidth - 210, 48);
	ctx.fillText(`Grid: ${snapshot.width}x${snapshot.height}`, canvasWidth - 210, 66);
	ctx.fillText(`Seed: ${snapshot.seed}`, canvasWidth - 210, 84);
	ctx.restore();
}
