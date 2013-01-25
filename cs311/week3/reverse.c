<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
	  "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title> CS311 - Operating Systems I </title>
<link rel="stylesheet" type="text/css" href="http://web.engr.oregonstate.edu/~dmcgrath/classes.css" />
</head>
<div id="Header"> CS311 - Operating Systems I  &nbsp; &nbsp; &nbsp;	D. Kevin McGrath &nbsp; &nbsp; &nbsp; MWF 08:00 - 08:50, WNGR 153 &nbsp; &nbsp; &nbsp; Winter 2013 </div>
<center><h2></h2></center>
<div id="Menu">
<a href="index.cgi?home=1">Home/Announcements</a> <br />
<a href="index.cgi?style=1">Style Guide</a> <br />
<a href="index.cgi?examples=1">Materials from Class</a> <br />
<a href="index.cgi?assignments=1">Assignments</a> <br />
<a href="index.cgi?grades=1">Grades</a> <br />
<a href="index.cgi?schedule=1">Schedule</a> <br />
<a href="index.cgi?resources=1">Other Resources</a> <br />
<a href="https://secure.engr.oregonstate.edu:8000/teach.php?type=assignment">Submission</a> <br />
<a href="index.cgi?file=syllabus">Syllabus</a> <br />
</div>
<div id="Content">
<a href=http://classes.engr.oregonstate.edu/eecs/winter2013/cs311/examples/reverse.c>Download file</a>
<div class="highlight"><pre><span class="cp">#include &lt;stdio.h&gt;</span>
<span class="cp">#include &lt;sys/types.h&gt;</span>
<span class="cp">#include &lt;fcntl.h&gt;</span>
<span class="cp">#include &lt;errno.h&gt;</span>
<span class="cp">#include &lt;stdlib.h&gt;</span>
<span class="cp">#include &lt;unistd.h&gt;</span>

<span class="cp">#define BLOCKSIZE 1</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">**</span><span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
	<span class="kt">char</span> <span class="o">*</span><span class="n">input</span> <span class="o">=</span> <span class="n">argv</span><span class="p">[</span><span class="mi">1</span><span class="p">];</span>
	<span class="kt">char</span> <span class="o">*</span><span class="n">output</span> <span class="o">=</span> <span class="n">argv</span><span class="p">[</span><span class="mi">2</span><span class="p">];</span>

	<span class="kt">int</span> <span class="n">in_fd</span><span class="p">;</span>
	<span class="kt">int</span> <span class="n">out_fd</span><span class="p">;</span>

	<span class="kt">char</span> <span class="n">buf</span><span class="p">[</span><span class="n">BLOCKSIZE</span><span class="p">];</span>

	<span class="kt">int</span> <span class="n">num_read</span><span class="p">;</span>
	<span class="kt">int</span> <span class="n">num_written</span><span class="p">;</span>

	<span class="kt">off_t</span> <span class="n">file_size</span><span class="p">;</span>
	<span class="kt">off_t</span> <span class="n">copied</span><span class="p">;</span>

	<span class="n">in_fd</span> <span class="o">=</span> <span class="n">open</span><span class="p">(</span><span class="n">input</span><span class="p">,</span> <span class="n">O_RDONLY</span><span class="p">);</span>
	<span class="k">if</span><span class="p">(</span><span class="n">in_fd</span> <span class="o">==</span> <span class="o">-</span><span class="mi">1</span><span class="p">){</span>
		<span class="n">perror</span><span class="p">(</span><span class="s">&quot;Can&#39;t open input file&quot;</span><span class="p">);</span>
		<span class="n">exit</span><span class="p">(</span><span class="o">-</span><span class="mi">1</span><span class="p">);</span>
	<span class="p">}</span>

	<span class="n">out_fd</span> <span class="o">=</span> <span class="n">open</span><span class="p">(</span><span class="n">output</span><span class="p">,</span> <span class="n">O_WRONLY</span> <span class="o">|</span> <span class="n">O_CREAT</span><span class="p">,</span> <span class="mo">0777</span><span class="p">);</span>
	<span class="k">if</span><span class="p">(</span><span class="n">out_fd</span> <span class="o">==</span> <span class="o">-</span><span class="mi">1</span><span class="p">){</span>
		<span class="n">perror</span><span class="p">(</span><span class="s">&quot;Can&#39;t open output file&quot;</span><span class="p">);</span>
		<span class="n">exit</span><span class="p">(</span><span class="o">-</span><span class="mi">1</span><span class="p">);</span>
	<span class="p">}</span>
	
	<span class="n">file_size</span> <span class="o">=</span> <span class="n">lseek</span><span class="p">(</span><span class="n">in_fd</span><span class="p">,</span> <span class="mi">0</span><span class="p">,</span> <span class="n">SEEK_END</span><span class="p">);</span>
	<span class="n">copied</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>
	<span class="n">lseek</span><span class="p">(</span><span class="n">in_fd</span><span class="p">,</span> <span class="o">-</span><span class="mi">1</span><span class="p">,</span> <span class="n">SEEK_END</span><span class="p">);</span>

	<span class="k">while</span><span class="p">(</span><span class="n">copied</span> <span class="o">&lt;</span> <span class="n">file_size</span><span class="p">){</span>
		<span class="n">num_read</span> <span class="o">=</span> <span class="n">read</span><span class="p">(</span><span class="n">in_fd</span><span class="p">,</span> <span class="n">buf</span><span class="p">,</span> <span class="n">BLOCKSIZE</span><span class="p">);</span>
		<span class="n">num_written</span> <span class="o">=</span> <span class="n">write</span><span class="p">(</span><span class="n">out_fd</span><span class="p">,</span> <span class="n">buf</span><span class="p">,</span> <span class="n">BLOCKSIZE</span><span class="p">);</span>

		<span class="k">if</span> <span class="p">(</span><span class="n">num_read</span> <span class="o">!=</span> <span class="n">num_written</span><span class="p">){</span>
			<span class="n">perror</span><span class="p">(</span><span class="s">&quot;Error writing file&quot;</span><span class="p">);</span>
			<span class="n">unlink</span><span class="p">(</span><span class="n">output</span><span class="p">);</span>
			<span class="n">exit</span><span class="p">(</span><span class="o">-</span><span class="mi">1</span><span class="p">);</span>
		<span class="p">}</span>

		<span class="n">copied</span> <span class="o">+=</span> <span class="n">num_written</span><span class="p">;</span>

		<span class="n">lseek</span><span class="p">(</span><span class="n">in_fd</span><span class="p">,</span> <span class="o">-</span><span class="mi">2</span><span class="p">,</span> <span class="n">SEEK_CUR</span><span class="p">);</span>
	<span class="p">}</span>
	
	<span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span>
</pre></div>

<hr>
&nbsp;&nbsp;&nbsp;<a href=mailto:dmcgrath@eecs.oregonstate.edu><i>D. Kevin McGrath</i></a>
<br>&nbsp;&nbsp;&nbsp;<i>Last modified:  Tue Jan 22 08:30:26 2013</i>
</div></body>
</html>
