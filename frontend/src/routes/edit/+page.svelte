<script lang="ts">
	import { onMount, onDestroy } from 'svelte';

	let blocklyDiv: HTMLDivElement;
	let workspace: unknown = null;
	let generatedSource = $state('');

	onMount(async () => {
		const BlocklyLib = await import('blockly');
		type WorkspaceSvg = InstanceType<typeof BlocklyLib.WorkspaceSvg>;
		const { registerBlocks, TOOLBOX_XML } = await import('$lib/blocks/definitions');
		const { createGenerator } = await import('$lib/blocks/generator');

		registerBlocks();
		const generator = createGenerator();

		workspace = BlocklyLib.inject(blocklyDiv, {
			toolbox: TOOLBOX_XML,
			grid: { spacing: 20, length: 3, colour: '#ccc', snap: true },
			zoom: { controls: true, wheel: true, startScale: 1.0 },
			trashcan: true
		});

		const ws = workspace as WorkspaceSvg;

		const mainBlock = ws.newBlock('gol_main');
		mainBlock.initSvg();
		mainBlock.render();
		mainBlock.moveBy(20, 20);

		const saved = localStorage.getItem('gol_workspace');
		if (saved) {
			try {
				BlocklyLib.serialization.workspaces.load(JSON.parse(saved), ws);
			} catch {
				// Ignore restore errors.
			}
		}

		ws.addChangeListener(() => {
			try {
				generatedSource = generator.workspaceToCode(workspace as WorkspaceSvg);
			} catch {
				generatedSource = '// Error generating code';
			}

			const state = BlocklyLib.serialization.workspaces.save(workspace as WorkspaceSvg);
			localStorage.setItem('gol_workspace', JSON.stringify(state));
		});

		generatedSource = generator.workspaceToCode(ws);
	});

	onDestroy(() => {
		if (workspace) {
			(workspace as { dispose: () => void }).dispose();
		}
	});

	// --- Submit flow ---
	import { createBot } from '$lib/api';
	import { goto } from '$app/navigation';

	let botName = $state('');
	let showSubmit = $state(false);
	let submitError = $state('');
	let compileErrors = $state<{ message: string; line: number; col: number }[]>([]);
	let submitting = $state(false);

	async function handleSubmit() {
		if (!botName.trim() || !generatedSource.trim()) return;
		submitError = '';
		compileErrors = [];
		submitting = true;
		try {
			const result = await createBot(botName, generatedSource);
			if (!result.compile_ok && result.compile_errors) {
				compileErrors = result.compile_errors;
			} else {
				goto(`/me/bots/${result.id}`);
			}
		} catch (err) {
			submitError = err instanceof Error ? err.message : 'Submit failed';
		} finally {
			submitting = false;
		}
	}
</script>

<svelte:head>
	<title>Editor - GameOfLifes</title>
</svelte:head>

<div class="editor-layout">
	<div bind:this={blocklyDiv} class="blockly-pane"></div>

	<div class="source-pane">
		<h3>Generated Source</h3>
		<pre class="source-code">{generatedSource}</pre>

		<div class="submit-section">
			{#if !showSubmit}
				<button class="btn-primary submit-toggle" onclick={() => (showSubmit = true)}>
					Submit Bot
				</button>
			{:else}
				<div class="submit-form">
					<label for="bot-name">Bot name</label>
					<input id="bot-name" type="text" bind:value={botName} placeholder="my-clever-bot" />
					<div class="submit-actions">
						<button
							class="btn-primary"
							onclick={handleSubmit}
							disabled={submitting || !botName.trim()}
						>
							{submitting ? 'Submitting...' : 'Submit'}
						</button>
						<button onclick={() => (showSubmit = false)}> Cancel </button>
					</div>
				</div>
			{/if}

			{#if submitError}
				<p class="error-text">{submitError}</p>
			{/if}

			{#if compileErrors.length > 0}
				<div class="compile-errors">
					<strong>Compile errors:</strong>
					{#each compileErrors as err}
						<div class="compile-error">[{err.line}:{err.col}] {err.message}</div>
					{/each}
				</div>
			{/if}
		</div>
	</div>
</div>

<style>
	.editor-layout {
		display: flex;
		height: calc(100vh - var(--nav-height));
	}

	.blockly-pane {
		flex: 0.7;
		height: 100%;
	}

	.source-pane {
		flex: 0.3;
		padding: var(--sp-4);
		overflow-y: auto;
		background: var(--bg-surface);
		border-left: 1px solid var(--border);
	}

	.source-pane h3 {
		font-size: var(--text-sm);
		text-transform: uppercase;
		letter-spacing: 0.05em;
		color: var(--text-muted);
		margin-bottom: var(--sp-3);
	}

	.source-code {
		white-space: pre-wrap;
		font-size: var(--text-sm);
		line-height: 1.6;
		margin-bottom: var(--sp-6);
	}

	.submit-section {
		border-top: 1px solid var(--border);
		padding-top: var(--sp-4);
	}

	.submit-toggle {
		width: 100%;
	}

	.submit-form {
		display: flex;
		flex-direction: column;
		gap: var(--sp-3);
	}

	.submit-actions {
		display: flex;
		gap: var(--sp-2);
	}

	.compile-errors {
		margin-top: var(--sp-3);
		font-size: var(--text-sm);
		color: var(--danger);
	}

	.compile-error {
		font-family: var(--font-mono);
		padding: var(--sp-1) 0;
	}
</style>
