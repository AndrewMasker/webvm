<script>
import WebVM from '$lib/WebVM.svelte';
import * as configObj from '/config_terminal';
import { tryPlausible } from '$lib/plausible.js';
function handleProcessCreated(processCount)
{
	// Log the first 5 processes, to get an idea of the level of interaction from the public
	if(processCount <= 5)
	{
		tryPlausible(`Process started: ${processCount}`);
	}
}
</script>

<!--
	The disk cache stores the blocks written while using the VM. Key it on the disk image so
	that each new deploy (whose image name includes the workflow run id) starts with a clean
	cache. With a fixed name, old blocks are replayed on top of a different ext2 image,
	corrupting the filesystem (stale files, and files that read as GitHub's HTML 404 page).
-->
<WebVM configObj={configObj} processCallback={handleProcessCreated} cacheId={`blocks_terminal_${configObj.diskImageUrl}`}>
	<p>Looking for a complete desktop experience? Try the new <a class="underline" href="/alpine.html" target="_blank">Alpine Linux</a> graphical WebVM</p>
</WebVM>
