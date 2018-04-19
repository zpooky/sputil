GTEST_ROOT = googletest/googletest/make

# gtest {{{
$(GTEST_ROOT)/gtest.a:
	$(MAKE) -C $(GTEST_ROOT) gtest.a -j
	cp $(GTEST_ROOT)/gtest.a $(GTEST_ROOT)/libgtest.a
# }}}


# vim:filetype=make
