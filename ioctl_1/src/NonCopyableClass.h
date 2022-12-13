
class NonCopyableClass {
	public:
		NonCopyableClass() = default;
		NonCopyableClass(const NonCopyableClass &) = delete;
		NonCopyableClass &operator=(const NonCopyableClass &) = delete;

		NonCopyableClass(NonCopyableClass &&) = default;
		NonCopyableClass &operator=(NonCopyableClass &&) = default;
};
