/** Runtime validation for Snapshot data received from WebSocket. */
import type { Snapshot } from './types';

export function isValidSnapshot(obj: unknown): obj is Snapshot {
	if (typeof obj !== 'object' || obj === null) return false;
	const o = obj as Record<string, unknown>;
	return (
		typeof o.version === 'number' &&
		typeof o.tick === 'number' &&
		typeof o.width === 'number' &&
		typeof o.height === 'number' &&
		typeof o.seed === 'number' &&
		Array.isArray(o.agents) &&
		typeof o.food === 'string' &&
		typeof o.water === 'string' &&
		typeof o.mineral === 'string' &&
		typeof o.terrain === 'string'
	);
}
