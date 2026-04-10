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
</script>

<svelte:head>
	<title>Editor - GameOfLifes</title>
</svelte:head>

<div style="display:flex;height:calc(100vh - 50px);">
	<div bind:this={blocklyDiv} style="flex:0.7;height:100%;"></div>
	<div style="flex:0.3;padding:1rem;overflow-y:auto;background:#1e1e1e;color:#d4d4d4;">
		<h3 style="margin-top:0;">Generated Source</h3>
		<pre style="white-space:pre-wrap;font-size:13px;line-height:1.5;">{generatedSource}</pre>
	</div>
</div>
