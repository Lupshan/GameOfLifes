<script lang="ts">
	import { page } from '$app/state';
</script>

<svelte:head>
	<title>Error {page.status} - GameOfLifes</title>
</svelte:head>

<div class="error-page">
	<div class="error-content">
		<span class="error-code">{page.status}</span>
		<h1>{page.status === 404 ? 'Page not found' : 'Something went wrong'}</h1>
		<p>{page.error?.message ?? 'An unexpected error occurred.'}</p>
		<a href="/" class="btn btn-primary">Back to home</a>
	</div>
</div>

<style>
	.error-page {
		display: flex;
		justify-content: center;
		align-items: center;
		min-height: calc(100vh - var(--nav-height));
		padding: var(--sp-4);
		text-align: center;
	}

	.error-content {
		max-width: 480px;
	}

	.error-code {
		font-size: 5rem;
		font-weight: 700;
		color: var(--accent);
		line-height: 1;
		display: block;
		margin-bottom: var(--sp-4);
	}

	h1 {
		margin-bottom: var(--sp-3);
	}

	p {
		margin-bottom: var(--sp-6);
	}
</style>
