<script lang="ts">
	import { onMount } from 'svelte';
	import { page } from '$app/state';
	import { getBot, publishBot } from '$lib/api';

	interface BotDetail {
		id: string;
		name: string;
		published: boolean;
		compile_ok: boolean;
		compile_errors?: { message: string; line: number; col: number }[];
		source: string;
	}

	let bot = $state<BotDetail | null>(null);
	let loading = $state(true);
	let publishError = $state('');
	let publishing = $state(false);

	onMount(async () => {
		try {
			bot = await getBot(page.params.id);
		} catch {
			bot = null;
		}
		loading = false;
	});

	async function handlePublish() {
		if (!bot) return;
		publishing = true;
		publishError = '';
		try {
			await publishBot(bot.id);
			bot = { ...bot, published: true };
		} catch (err) {
			publishError = err instanceof Error ? err.message : 'Publish failed';
		} finally {
			publishing = false;
		}
	}
</script>

<svelte:head>
	<title>{bot?.name ?? 'Bot'} - GameOfLifes</title>
</svelte:head>

<div style="max-width:800px;margin:2rem auto;padding:1rem;">
	{#if loading}
		<p>Loading...</p>
	{:else if !bot}
		<p>Bot not found.</p>
	{:else}
		<h1>{bot.name}</h1>

		<div style="display:flex;gap:1rem;margin-bottom:1rem;">
			<span>Compiled: {bot.compile_ok ? '✓' : '✗'}</span>
			<span>Published: {bot.published ? '✓' : '—'}</span>
		</div>

		{#if bot.compile_ok && !bot.published}
			<button onclick={handlePublish} disabled={publishing}
				style="padding:0.5rem 1rem;background:#4caf50;color:#fff;border:none;cursor:pointer;margin-bottom:1rem;">
				{publishing ? 'Publishing...' : 'Publish to World'}
			</button>
			{#if publishError}
				<p style="color:red;">{publishError}</p>
			{/if}
		{/if}

		{#if bot.published}
			<p style="color:#4caf50;">Published! <a href="/world">View in World</a></p>
		{/if}

		{#if bot.compile_errors && bot.compile_errors.length > 0}
			<div style="margin-bottom:1rem;color:red;">
				<strong>Compile errors:</strong>
				{#each bot.compile_errors as err}
					<p>[{err.line}:{err.col}] {err.message}</p>
				{/each}
			</div>
		{/if}

		<h3>Source</h3>
		<pre style="background:#1e1e1e;color:#d4d4d4;padding:1rem;overflow-x:auto;">{bot.source}</pre>

		<p style="margin-top:1rem;"><a href="/me/bots">← Back to My Bots</a></p>
	{/if}
</div>
