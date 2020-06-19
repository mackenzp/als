.PHONY: all clean

abc = abc
error = error

all:
	@echo "**********< Compiling abc logic synthesis tool >**********"
	$(MAKE) -C $(abc)
	@echo "\n\n**********< Compiling error calculation module >**********"
	$(MAKE) -C $(error)
	
	
clean:
	@echo "**********< Cleaning abc >**********"
	$(MAKE) -C $(abc) clean
	@echo "\n\n**********< Cleaning error calc module >**********"
	$(MAKE) -C $(error) clean
	

