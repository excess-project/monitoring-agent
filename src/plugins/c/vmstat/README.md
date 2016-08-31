# EXCESS ATOM Monitoring Framework: /prov/vmstat Plug-in

The /proc/vmstat plug-in reports on I/O statistics. A usage example is found in
the subfolder **src/utils**.



## Usage

The /proc/vmstat plug-in can be build and ran outside the EXCESS monitoring
framework. Please execute the **MAKEFILE** using

```
$ make
```

Next, you can execute the demo client **mf_vmstat_client** as follows:

```
$ ./mf_vmstat_client <LIST_OF_METRICS>
```

Replace **<LIST_OF_METRICS>** with a space-separated list of valid /proc/vmstat
events. It follows a list of supported events in the next section.



## List of supported events

- nr_alloc_batch
- nr_inactive_anon
- nr_active_anon
- nr_inactive_file
- nr_active_file
- nr_unevictable
- nr_mlock
- nr_anon_pages
- nr_mapped
- nr_file_pages
- nr_dirty
- nr_writeback
- nr_slab_reclaimable
- nr_slab_unreclaimable
- nr_page_table_pages
- nr_kernel_stack
- nr_unstable
- nr_bounce
- nr_vmscan_write
- nr_vmscan_immediate_reclaim
- nr_writeback_temp
- nr_isolated_anon
- nr_isolated_file
- nr_shmem
- nr_dirtied
- nr_written
- numa_hit
- numa_miss
- numa_foreign
- numa_interleave
- numa_local
- numa_other
- nr_anon_transparent_hugepages
- nr_free_cma
- nr_dirty_threshold
- nr_dirty_background_threshold
- pgpgin
- pgpgout
- pswpin
- pswpout
- pgalloc_dma
- pgalloc_dma32
- pgalloc_normal
- pgalloc_movable
- pgfree
- pgactivate
- pgdeactivate
- pgfault
- pgmajfault
- pgrefill_dma
- pgrefill_dma32
- pgrefill_normal
- pgrefill_movable
- pgsteal_kswapd_dma
- pgsteal_kswapd_dma32
- pgsteal_kswapd_normal
- pgsteal_kswapd_movable
- pgsteal_direct_dma
- pgsteal_direct_dma32
- pgsteal_direct_normal
- pgsteal_direct_movable
- pgscan_kswapd_dma
- pgscan_kswapd_dma32
- pgscan_kswapd_normal
- pgscan_kswapd_movable
- pgscan_direct_dma
- pgscan_direct_dma32
- pgscan_direct_normal
- pgscan_direct_movable
- pgscan_direct_throttle
- zone_reclaim_failed
- pginodesteal
- slabs_scanned
- kswapd_inodesteal
- kswapd_low_wmark_hit_quickly
- kswapd_high_wmark_hit_quickly
- pageoutrun
- allocstall
- pgrotated
- numa_pte_updates
- numa_huge_pte_updates
- numa_hint_faults
- numa_hint_faults_local
- numa_pages_migrated
- pgmigrate_success
- pgmigrate_fail
- compact_migrate_scanned
- compact_free_scanned
- compact_isolated
- compact_stall
- compact_fail
- compact_success
- htlb_buddy_alloc_success
- htlb_buddy_alloc_fail
- unevictable_pgs_culled
- unevictable_pgs_scanned
- unevictable_pgs_rescued
- unevictable_pgs_mlocked
- unevictable_pgs_munlocked
- unevictable_pgs_cleared
- unevictable_pgs_stranded
- thp_fault_alloc
- thp_fault_fallback
- thp_collapse_alloc
- thp_collapse_alloc_failed
- thp_split
- thp_zero_page_alloc
- thp_zero_page_alloc_failed
- nr_tlb_remote_flush
- nr_tlb_remote_flush_received
- nr_tlb_local_flush_all
- nr_tlb_local_flush_one
