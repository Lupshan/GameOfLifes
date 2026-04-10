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
			bot = await getBot(page.params.id!);
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

<div class="container">
	{#if loading}
		<p class="loading">Loading bot...</p>
	{:else if !bot}
		<div class="card empty-state">
			<p>Bot not found.</p>
			<a href="/me/bots" class="btn">Back to My Bots</a>
		</div>
	{:else}
		<div class="page-header">
			<div>
				<h1>{bot.name}</h1>
				<div class="status-row">
					<span
						class="badge"
						class:badge-success={bot.compile_ok}
						class:badge-danger={!bot.compile_ok}
					>
						{bot.compile_ok ? 'Compiled' : 'Compile error'}
					</span>
					<span class="badge" class:badge-success={bot.published}>
						{bot.published ? 'Published' : 'Draft'}
					</span>
				</div>
			</div>
		</div>

		{#if bot.compile_ok && !bot.published}
			<div class="publish-bar">
				<button onclick={handlePublish} disabled={publishing} class="btn-primary">
					{publishing ? 'Publishing...' : 'Publish to World'}
				</button>
				{#if publishError}
					<p class="error-text">{publishError}</p>
				{/if}
			</div>
		{/if}

		{#if bot.published}
			<div class="published-banner">
				Published and live! <a href="/world">View in World</a>
			</div>
		{/if}

		{#if bot.compile_errors && bot.compile_errors.length > 0}
			<div class="errors-card card">
				<h3>Compile Errors</h3>
				{#each bot.compile_errors as err}
					<div class="error-line">
						<span class="error-loc">{err.line}:{err.col}</span>
						<span>{err.message}</span>
					</div>
				{/each}
			</div>
		{/if}

		<div class="source-section card">
			<h3>Source</h3>
			<pre>{bot.source}</pre>
		</div>

		<a href="/me/bots" class="back-link">&larr; Back to My Bots</a>
	{/if}
</div>

<style>
	.page-header {
		margin-bottom: var(--sp-6);
	}

	.page-header h1 {
		margin-bottom: var(--sp-2);
	}

	.status-row {
		display: flex;
		gap: var(--sp-2);
	}

	.publish-bar {
		margin-bottom: var(--sp-4);
	}

	.published-banner {
		background: var(--accent-glow);
		color: var(--accent);
		border: 1px solid var(--accent);
		border-radius: var(--radius);
		padding: var(--sp-3) var(--sp-4);
		margin-bottom: var(--sp-4);
		font-weight: 500;
	}

	.published-banner a {
		color: var(--accent);
		text-decoration: underline;
	}

	.errors-card {
		border-color: var(--danger);
		margin-bottom: var(--sp-4);
	}

	.errors-card h3 {
		color: var(--danger);
	}

	.error-line {
		padding: var(--sp-2) 0;
		border-bottom: 1px solid var(--border);
		font-size: var(--text-sm);
		color: var(--danger);
	}

	.error-line:last-child {
		border-bottom: none;
	}

	.error-loc {
		font-family: var(--font-mono);
		font-weight: 600;
		margin-right: var(--sp-3);
		color: var(--text-muted);
	}

	.source-section {
		margin-bottom: var(--sp-6);
	}

	.source-section pre {
		margin-top: var(--sp-3);
		margin-bottom: 0;
	}

	.back-link {
		font-size: var(--text-sm);
	}

	.empty-state {
		text-align: center;
		padding: var(--sp-12);
	}
</style>
