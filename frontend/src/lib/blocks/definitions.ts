/** Custom Blockly block definitions for GoL Script. */
import * as Blockly from 'blockly';

/** Register all custom GoL Script blocks. */
export function registerBlocks(): void {
	// --- Main function ---
	Blockly.Blocks['gol_main'] = {
		init(this: Blockly.Block) {
			this.appendDummyInput().appendField('fn main()');
			this.appendStatementInput('BODY').setCheck(null);
			this.setColour(290);
			this.setDeletable(false);
		}
	};

	// --- Actions ---
	const actions = [
		{ type: 'gol_move', label: 'move()', colour: 160 },
		{ type: 'gol_turn_left', label: 'turn_left()', colour: 160 },
		{ type: 'gol_turn_right', label: 'turn_right()', colour: 160 },
		{ type: 'gol_eat', label: 'eat()', colour: 120 },
		{ type: 'gol_drink', label: 'drink()', colour: 120 },
		{ type: 'gol_reproduce', label: 'reproduce()', colour: 330 }
	];

	for (const a of actions) {
		Blockly.Blocks[a.type] = {
			init(this: Blockly.Block) {
				this.appendDummyInput().appendField(a.label);
				this.setPreviousStatement(true, null);
				this.setNextStatement(true, null);
				this.setColour(a.colour);
			}
		};
	}

	// --- Perception (value blocks) ---
	const perceptions = [
		{ type: 'gol_perceive_food', label: 'perceive_food()' },
		{ type: 'gol_perceive_water', label: 'perceive_water()' },
		{ type: 'gol_perceive_terrain', label: 'perceive_terrain()' },
		{ type: 'gol_look_food', label: 'look_food()' },
		{ type: 'gol_look_water', label: 'look_water()' },
		{ type: 'gol_my_energy', label: 'my_energy()' },
		{ type: 'gol_my_hydration', label: 'my_hydration()' }
	];

	for (const p of perceptions) {
		Blockly.Blocks[p.type] = {
			init(this: Blockly.Block) {
				this.appendDummyInput().appendField(p.label);
				this.setOutput(true, 'Number');
				this.setColour(230);
			}
		};
	}

	// --- my_trait(id) ---
	Blockly.Blocks['gol_my_trait'] = {
		init(this: Blockly.Block) {
			this.appendDummyInput()
				.appendField('my_trait(')
				.appendField(
					new Blockly.FieldDropdown([
						['vision', '0'],
						['speed', '1'],
						['metabolism', '2'],
						['repro_threshold', '3'],
						['swim', '4'],
						['forage', '5']
					]),
					'TRAIT_ID'
				)
				.appendField(')');
			this.setOutput(true, 'Number');
			this.setColour(230);
		}
	};

	// --- Variable declaration ---
	Blockly.Blocks['gol_let'] = {
		init(this: Blockly.Block) {
			this.appendValueInput('VALUE').appendField('let').appendField(
				new Blockly.FieldTextInput('x'),
				'NAME'
			).appendField('=');
			this.setPreviousStatement(true, null);
			this.setNextStatement(true, null);
			this.setColour(60);
		}
	};

	// --- Variable assignment ---
	Blockly.Blocks['gol_set'] = {
		init(this: Blockly.Block) {
			this.appendValueInput('VALUE').appendField(
				new Blockly.FieldTextInput('x'),
				'NAME'
			).appendField('=');
			this.setPreviousStatement(true, null);
			this.setNextStatement(true, null);
			this.setColour(60);
		}
	};

	// --- Variable read ---
	Blockly.Blocks['gol_var'] = {
		init(this: Blockly.Block) {
			this.appendDummyInput().appendField(new Blockly.FieldTextInput('x'), 'NAME');
			this.setOutput(true, null);
			this.setColour(60);
		}
	};

	// --- If/else ---
	Blockly.Blocks['gol_if'] = {
		init(this: Blockly.Block) {
			this.appendValueInput('COND').setCheck('Boolean').appendField('if');
			this.appendStatementInput('THEN');
			this.appendStatementInput('ELSE').appendField('else');
			this.setPreviousStatement(true, null);
			this.setNextStatement(true, null);
			this.setColour(210);
		}
	};

	// --- While ---
	Blockly.Blocks['gol_while'] = {
		init(this: Blockly.Block) {
			this.appendValueInput('COND').setCheck('Boolean').appendField('while');
			this.appendStatementInput('BODY');
			this.setPreviousStatement(true, null);
			this.setNextStatement(true, null);
			this.setColour(210);
		}
	};
}

/** Toolbox XML for the block editor. */
export const TOOLBOX_XML = `
<xml>
  <category name="Control" colour="210">
    <block type="gol_if"></block>
    <block type="gol_while"></block>
  </category>
  <category name="Actions" colour="160">
    <block type="gol_move"></block>
    <block type="gol_turn_left"></block>
    <block type="gol_turn_right"></block>
    <block type="gol_eat"></block>
    <block type="gol_drink"></block>
    <block type="gol_reproduce"></block>
  </category>
  <category name="Perception" colour="230">
    <block type="gol_perceive_food"></block>
    <block type="gol_perceive_water"></block>
    <block type="gol_perceive_terrain"></block>
    <block type="gol_look_food"></block>
    <block type="gol_look_water"></block>
    <block type="gol_my_energy"></block>
    <block type="gol_my_hydration"></block>
    <block type="gol_my_trait"></block>
  </category>
  <category name="Variables" colour="60">
    <block type="gol_let"></block>
    <block type="gol_set"></block>
    <block type="gol_var"></block>
  </category>
  <category name="Math" colour="230">
    <block type="math_number"></block>
    <block type="math_arithmetic"></block>
    <block type="logic_compare"></block>
    <block type="logic_operation"></block>
    <block type="logic_negate"></block>
    <block type="logic_boolean"></block>
  </category>
</xml>`;
