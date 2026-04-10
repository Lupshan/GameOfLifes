<script lang="ts">
	import { auth } from '$lib/stores/auth';
	import { apiFetch } from '$lib/api';
	import { onMount } from 'svelte';
	import { goto } from '$app/navigation';

	interface Bot {
		id: string;
		name: string;
		published: boolean;
		compile_ok: boolean;
	}

	let bots = $state<Bot[]>([]);
	let loading = $state(true);

	onMount(async () => {
		if (!$auth.loggedIn) {
			goto('/login');
			return;
		}
		const resp = await apiFetch('/bots');
		if (resp.ok) {
			bots = await resp.json();
		}
		loading = false;
	});
</script>

<svelte:head>
	<title>My Bots - GameOfLifes</title>
</svelte:head>

<div class="container">
	<div class="page-header">
		<h1>My Bots</h1>
		<a href="/edit" class="btn btn-primary">+ New Bot</a>
	</div>

	{#if loading}
		<p class="loading">Loading your bots...</p>
	{:else if bots.length === 0}
		<div class="empty-state card">
			<p>You haven't created any bots yet.</p>
			<a href="/edit" class="btn btn-primary">Create your first bot</a>
		</div>
	{:else}
		<div class="card">
			<table>
				<thead>
					<tr>
						<th>Name</th>
						<th>Compiled</th>
						<th>Published</th>
						<th>Actions</th>
					</tr>
				</thead>
				<tbody>
					{#each bots as bot}
						<tr>
							<td><strong>{bot.name}</strong></td>
							<td>
								<span
									class="badge"
									class:badge-success={bot.compile_ok}
									class:badge-danger={!bot.compile_ok}
								>
									{bot.compile_ok ? 'OK' : 'Error'}
								</span>
							</td>
							<td>
								<span class="badge" class:badge-success={bot.published}>
									{bot.published ? 'Live' : 'Draft'}
								</span>
							</td>
							<td>
								<a href="/me/bots/{bot.id}" class="btn btn-ghost">View</a>
							</td>
						</tr>
					{/each}
				</tbody>
			</table>
		</div>
	{/if}
</div>

<style>
	.page-header {
		display: flex;
		justify-content: space-between;
		align-items: center;
		margin-bottom: var(--sp-6);
	}

	.page-header h1 {
		margin-bottom: 0;
	}

	.empty-state {
		text-align: center;
		padding: var(--sp-12);
	}

	.empty-state p {
		margin-bottom: var(--sp-4);
	}
</style>
