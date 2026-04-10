/** Snapshot data received from the WebSocket. */
export interface SnapshotAgent {
	id: number;
	x: number;
	y: number;
	energy: number;
	hydration: number;
	alive: boolean;
	genome: string;
	parent_id: number;
	generation: number;
}

export interface Snapshot {
	version: number;
	tick: number;
	width: number;
	height: number;
	seed: number;
	agents: SnapshotAgent[];
	food: string; // base64
	water: string; // base64
	mineral: string; // base64
	terrain: string; // base64
	event?: string;
}

/** Camera state for pan+zoom. */
export interface Camera {
	x: number;
	y: number;
	zoom: number;
}
