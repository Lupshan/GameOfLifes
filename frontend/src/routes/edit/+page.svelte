<script lang="ts">
	import { onMount, onDestroy } from 'svelte';

	let blocklyDiv: HTMLDivElement;
	let workspace: unknown = null;
	let generatedSource = $state('');

	onMount(async () => {
		const BlocklyLib = await import('blockly');
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

		// Add default main block.
		const mainBlock = (workspace as BlocklyLib.WorkspaceSvg).newBlock('gol_main');
		mainBlock.initSvg();
		mainBlock.render();
		mainBlock.moveBy(20, 20);

		// Restore saved workspace.
		const saved = localStorage.getItem('gol_workspace');
		if (saved) {
			try {
				BlocklyLib.serialization.workspaces.load(JSON.parse(saved), workspace as BlocklyLib.WorkspaceSvg);
			} catch {
				// Ignore restore errors.
			}
		}

		// Generate code on change.
		(workspace as BlocklyLib.WorkspaceSvg).addChangeListener(() => {
			try {
				generatedSource = generator.workspaceToCode(workspace as BlocklyLib.WorkspaceSvg);
			} catch {
				generatedSource = '// Error generating code';
			}

			// Auto-save workspace.
			const state = BlocklyLib.serialization.workspaces.save(workspace as BlocklyLib.WorkspaceSvg);
			localStorage.setItem('gol_workspace', JSON.stringify(state));
		});

		// Trigger initial generation.
		generatedSource = generator.workspaceToCode(workspace as BlocklyLib.WorkspaceSvg);
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

<div style="display:flex;height:calc(100vh - 50px);">
	<div bind:this={blocklyDiv} style="flex:0.7;height:100%;"></div>
	<div style="flex:0.3;padding:1rem;overflow-y:auto;background:#1e1e1e;color:#d4d4d4;">
		<h3 style="margin-top:0;">Generated Source</h3>
		<pre style="white-space:pre-wrap;font-size:13px;line-height:1.5;">{generatedSource}</pre>

		<hr style="border-color:#444;margin:1rem 0;" />

		{#if !showSubmit}
			<button onclick={() => showSubmit = true}
				style="padding:0.5rem 1rem;cursor:pointer;background:#4caf50;color:#fff;border:none;">
				Submit Bot
			</button>
		{:else}
			<div>
				<label for="bot-name" style="display:block;margin-bottom:0.25rem;">Bot name:</label>
				<input id="bot-name" type="text" bind:value={botName}
					style="width:100%;padding:0.5rem;margin-bottom:0.5rem;" />
				<button onclick={handleSubmit} disabled={submitting || !botName.trim()}
					style="padding:0.5rem 1rem;cursor:pointer;background:#4caf50;color:#fff;border:none;">
					{submitting ? 'Submitting...' : 'Submit'}
				</button>
				<button onclick={() => showSubmit = false}
					style="padding:0.5rem 1rem;cursor:pointer;margin-left:0.5rem;">
					Cancel
				</button>
			</div>
		{/if}

		{#if submitError}
			<p style="color:#f44336;margin-top:0.5rem;">{submitError}</p>
		{/if}

		{#if compileErrors.length > 0}
			<div style="margin-top:0.5rem;color:#f44336;">
				<strong>Compile errors:</strong>
				{#each compileErrors as err}
					<p style="margin:0.25rem 0;">[{err.line}:{err.col}] {err.message}</p>
				{/each}
			</div>
		{/if}
	</div>
</div>
