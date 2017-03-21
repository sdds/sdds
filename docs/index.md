---
title: sensornetwork Data Distribution Service
---

sDDS - the “sensornet Data Distribution Service“ - is a next-generation
open source IoT middleware. It is an enabling technology for future
scalable IoT infrastructures supporting a broad range of devices.

sDDS is going to be released as open source on the CeBIT 2017.

Unitl then we are going to update this site with news etc.

## Blog

{% for post in site.posts %}
  <h2><a href="{{ post.url }}">{{ post.title }}</a></h2>
  <p class="author">
    <span class="date">{{ post.date }}</span>
  </p>
  <div class="content">
    {{ post.content }}
  </div>
{% endfor %}

<!-- Pagination links -->
<!--
<div class="pagination">
  {% if paginator.previous_page %}
    <a href="{{ paginator.previous_page_path }}" class="previous">Previous</a>
  {% else %}
    <span class="previous">Previous</span>
  {% endif %}
  <span class="page_number ">Page: {{ paginator.page }} of {{ paginator.total_pages }}</span>
  {% if paginator.next_page %}
    <a href="{{ paginator.next_page_path }}" class="next">Next</a>
  {% else %}
    <span class="next ">Next</span>
  {% endif %}
</div>
-->
