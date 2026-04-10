import { describe, it, expect } from 'vitest';

/** Simple playback state machine for testing. */
interface PlaybackState {
	playing: boolean;
	speed: number;
	currentIndex: number;
	maxIndex: number;
}

function tick(state: PlaybackState): PlaybackState {
	if (!state.playing) return state;
	const next = state.currentIndex + 1;
	if (next > state.maxIndex) {
		return { ...state, playing: false };
	}
	return { ...state, currentIndex: next };
}

function setSpeed(state: PlaybackState, speed: number): PlaybackState {
	return { ...state, speed };
}

function togglePlay(state: PlaybackState): PlaybackState {
	return { ...state, playing: !state.playing };
}

describe('playback state machine', () => {
	it('advances index on tick when playing', () => {
		const s = tick({ playing: true, speed: 1, currentIndex: 0, maxIndex: 10 });
		expect(s.currentIndex).toBe(1);
	});

	it('stops at end', () => {
		const s = tick({ playing: true, speed: 1, currentIndex: 10, maxIndex: 10 });
		expect(s.playing).toBe(false);
		expect(s.currentIndex).toBe(10);
	});

	it('does not advance when paused', () => {
		const s = tick({ playing: false, speed: 1, currentIndex: 5, maxIndex: 10 });
		expect(s.currentIndex).toBe(5);
	});

	it('toggle play/pause', () => {
		const s1 = togglePlay({ playing: false, speed: 1, currentIndex: 0, maxIndex: 10 });
		expect(s1.playing).toBe(true);
		const s2 = togglePlay(s1);
		expect(s2.playing).toBe(false);
	});

	it('set speed', () => {
		const s = setSpeed({ playing: true, speed: 1, currentIndex: 0, maxIndex: 10 }, 4);
		expect(s.speed).toBe(4);
	});
});
