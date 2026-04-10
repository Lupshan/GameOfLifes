/** GoL Script code generator from Blockly workspace. */
import * as Blockly from 'blockly';

type BlockCodeFn = (block: Blockly.Block) => string | [string, number];

const ORDER_NONE = 0;
const ORDER_ATOMIC = 99;

/** Create a GoL Script generator. */
export function createGenerator(): Blockly.CodeGenerator {
	const gen = new Blockly.CodeGenerator('GoLScript');

	// --- Main ---
	(gen as unknown as Record<string, BlockCodeFn>)['gol_main'] = (block) => {
		const body = gen.statementToCode(block, 'BODY');
		return `fn main() {\n${body}}\n`;
	};

	// --- Actions (statement blocks) ---
	const stmtBlocks: Record<string, string> = {
		gol_move: 'move();',
		gol_turn_left: 'turn_left();',
		gol_turn_right: 'turn_right();',
		gol_eat: 'eat();',
		gol_drink: 'drink();',
		gol_reproduce: 'reproduce();'
	};

	for (const [type, code] of Object.entries(stmtBlocks)) {
		(gen as unknown as Record<string, BlockCodeFn>)[type] = () => `${code}\n`;
	}

	// --- Perception (value blocks) ---
	const valBlocks: Record<string, string> = {
		gol_perceive_food: 'perceive_food()',
		gol_perceive_water: 'perceive_water()',
		gol_perceive_terrain: 'perceive_terrain()',
		gol_look_food: 'look_food()',
		gol_look_water: 'look_water()',
		gol_my_energy: 'my_energy()',
		gol_my_hydration: 'my_hydration()'
	};

	for (const [type, code] of Object.entries(valBlocks)) {
		(gen as unknown as Record<string, BlockCodeFn>)[type] = (): [string, number] => [
			code,
			ORDER_ATOMIC
		];
	}

	// --- my_trait ---
	(gen as unknown as Record<string, BlockCodeFn>)['gol_my_trait'] = (block): [string, number] => {
		const traitId = block.getFieldValue('TRAIT_ID');
		return [`my_trait(${traitId})`, ORDER_ATOMIC];
	};

	// --- Variables ---
	(gen as unknown as Record<string, BlockCodeFn>)['gol_let'] = (block) => {
		const name = block.getFieldValue('NAME');
		const value = gen.valueToCode(block, 'VALUE', ORDER_NONE) || '0';
		return `let ${name} = ${value};\n`;
	};

	(gen as unknown as Record<string, BlockCodeFn>)['gol_set'] = (block) => {
		const name = block.getFieldValue('NAME');
		const value = gen.valueToCode(block, 'VALUE', ORDER_NONE) || '0';
		return `${name} = ${value};\n`;
	};

	(gen as unknown as Record<string, BlockCodeFn>)['gol_var'] = (block): [string, number] => {
		const name = block.getFieldValue('NAME');
		return [name, ORDER_ATOMIC];
	};

	// --- Control ---
	(gen as unknown as Record<string, BlockCodeFn>)['gol_if'] = (block) => {
		const cond = gen.valueToCode(block, 'COND', ORDER_NONE) || 'false';
		const thenBranch = gen.statementToCode(block, 'THEN');
		const elseBranch = gen.statementToCode(block, 'ELSE');
		let code = `if (${cond}) {\n${thenBranch}}`;
		if (elseBranch) {
			code += ` else {\n${elseBranch}}`;
		}
		return code + '\n';
	};

	(gen as unknown as Record<string, BlockCodeFn>)['gol_while'] = (block) => {
		const cond = gen.valueToCode(block, 'COND', ORDER_NONE) || 'false';
		const body = gen.statementToCode(block, 'BODY');
		return `while (${cond}) {\n${body}}\n`;
	};

	// --- Blockly built-in math/logic blocks ---
	(gen as unknown as Record<string, BlockCodeFn>)['math_number'] = (block): [string, number] => {
		return [String(block.getFieldValue('NUM')), ORDER_ATOMIC];
	};

	(gen as unknown as Record<string, BlockCodeFn>)['math_arithmetic'] = (
		block
	): [string, number] => {
		const ops: Record<string, string> = {
			ADD: '+',
			MINUS: '-',
			MULTIPLY: '*',
			DIVIDE: '/',
			MODULO: '%'
		};
		const op = ops[block.getFieldValue('OP')] || '+';
		const a = gen.valueToCode(block, 'A', ORDER_NONE) || '0';
		const b = gen.valueToCode(block, 'B', ORDER_NONE) || '0';
		return [`${a} ${op} ${b}`, ORDER_NONE];
	};

	(gen as unknown as Record<string, BlockCodeFn>)['logic_compare'] = (
		block
	): [string, number] => {
		const ops: Record<string, string> = {
			EQ: '==',
			NEQ: '!=',
			LT: '<',
			LTE: '<=',
			GT: '>',
			GTE: '>='
		};
		const op = ops[block.getFieldValue('OP')] || '==';
		const a = gen.valueToCode(block, 'A', ORDER_NONE) || '0';
		const b = gen.valueToCode(block, 'B', ORDER_NONE) || '0';
		return [`${a} ${op} ${b}`, ORDER_NONE];
	};

	(gen as unknown as Record<string, BlockCodeFn>)['logic_operation'] = (
		block
	): [string, number] => {
		const op = block.getFieldValue('OP') === 'AND' ? '&&' : '||';
		const a = gen.valueToCode(block, 'A', ORDER_NONE) || 'false';
		const b = gen.valueToCode(block, 'B', ORDER_NONE) || 'false';
		return [`${a} ${op} ${b}`, ORDER_NONE];
	};

	(gen as unknown as Record<string, BlockCodeFn>)['logic_negate'] = (
		block
	): [string, number] => {
		const val = gen.valueToCode(block, 'BOOL', ORDER_NONE) || 'false';
		return [`!${val}`, ORDER_NONE];
	};

	(gen as unknown as Record<string, BlockCodeFn>)['logic_boolean'] = (
		block
	): [string, number] => {
		return [block.getFieldValue('BOOL') === 'TRUE' ? 'true' : 'false', ORDER_ATOMIC];
	};

	return gen;
}
