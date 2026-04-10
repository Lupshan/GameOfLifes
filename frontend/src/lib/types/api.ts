/** Centralized API response types. */

export const NO_PARENT = 18446744073709551615n;

export interface BotResponse {
	id: string;
	name: string;
	published: boolean;
	compile_ok: boolean;
	compile_errors?: CompileError[];
}

export interface BotDetailResponse extends BotResponse {
	source: string;
}

export interface CompileError {
	message: string;
	line: number;
	col: number;
}

export interface LineageEvent {
	id: string;
	tick: number;
	event_type: string;
	agent_id: number;
	bot_id: string | null;
	parent_agent_id: number | null;
	generation: number | null;
	cause: string | null;
}

export interface SnapshotMeta {
	id: string;
	tick: number;
	file_path: string;
	size_bytes: number;
}

export interface Tournament {
	id: string;
	name: string;
	description: string;
	status: string;
	max_ticks: number;
	scoring: string;
	leaderboard: { bot_name: string; score: number }[];
}

export interface Sim {
	id: string;
	name: string;
	status: string;
	max_ticks: number;
	created_at: string;
}
