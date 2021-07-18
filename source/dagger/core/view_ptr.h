#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace jss
{
	namespace detail
	{
		/// Detect the presence of operator*, operator-> and .get() for the
		/// given type
		template<typename Ptr, bool = std::is_class<Ptr>::value>
		struct HasSmartPointerOps
		{
			using FalseTest = char;
			template<typename Archetype>
			struct TrueTest
			{
				FalseTest dummy[2];
			};

			template<typename Archetype>
			static FalseTest TestOpStar(...);
			template<typename Archetype>
			static TrueTest<decltype(*std::declval<Archetype const&>())> TestOpStar(Archetype*);

			template<typename Archetype>
			static FalseTest TestOpArrow(...);
			template<typename Archetype>
			static TrueTest<decltype(std::declval<Archetype const&>().operator->())> TestOpArrow(Archetype*);

			template<typename Archetype>
			static FalseTest TestGet(...);
			template<typename Archetype>
			static TrueTest<decltype(std::declval<Archetype const&>().get())> TestGet(Archetype*);

			static constexpr bool s_Value = !std::is_same<decltype(TestGet<Ptr>(0)), FalseTest>::value &&
											!std::is_same<decltype(TestOpArrow<Ptr>(0)), FalseTest>::value &&
											!std::is_same<decltype(TestOpStar<Ptr>(0)), FalseTest>::value;
		};

		/// Non-class types can't be smart pointers
		template<typename Ptr>
		struct HasSmartPointerOps<Ptr, false> : std::false_type
		{
		};

		/// Ensure that the smart pointer operations give consistent return
		/// types
		template<typename Ptr>
		struct SmartPointerOpsConsistent
			: std::integral_constant<
				  bool,
				  std::is_pointer<decltype(std::declval<Ptr const&>().get())>::value &&
					  std::is_reference<decltype(*std::declval<Ptr const&>())>::value &&
					  std::is_pointer<decltype(std::declval<Ptr const&>().operator->())>::value &&
					  std::is_same<
						  decltype(std::declval<Ptr const&>().get()),
						  decltype(std::declval<Ptr const&>().operator->())>::value &&
					  std::is_same<
						  decltype(*std::declval<Ptr const&>().get()), decltype(*std::declval<Ptr const&>())>::value>
		{
		};

		/// Assume Ptr is a smart pointer if it has the relevant ops and they
		/// are consistent
		template<typename Ptr, bool = HasSmartPointerOps<Ptr>::value>
		struct IsSmartPointer : std::integral_constant<bool, SmartPointerOpsConsistent<Ptr>::value>
		{
		};

		/// If Ptr doesn't have the relevant ops then it can't be a smart
		/// pointer
		template<typename Ptr>
		struct IsSmartPointer<Ptr, false> : std::false_type
		{
		};

		/// Check if Ptr is a smart pointer that holds a pointer convertible to
		/// T*
		template<typename Ptr, typename Archetype, bool = IsSmartPointer<Ptr>::value>
		struct IsConvertibleSmartPointer
			: std::integral_constant<
				  bool, std::is_convertible<decltype(std::declval<Ptr const&>().get()), Archetype*>::value>
		{
		};

		/// If Ptr isn't a smart pointer then we don't want it
		template<typename Ptr, typename Archetype>
		struct IsConvertibleSmartPointer<Ptr, Archetype, false> : std::false_type
		{
		};

	} // namespace detail

	/// A basic "smart" pointer that points to an individual object it does not
	/// own. Unlike a raw pointer, it does not support pointer arithmetic or
	/// array operations, and the pointee cannot be accidentally deleted. It
	/// supports implicit conversion from any smart pointer that holds a pointer
	/// convertible to T*
	template<typename Archetype>
	class ObjectPtr
	{
	public:
		/// Construct a null pointer
		constexpr ObjectPtr() noexcept : m_Ptr(nullptr) { }
		/// Construct a null pointer
		constexpr ObjectPtr(std::nullptr_t) noexcept : m_Ptr(nullptr) { }
		/// Construct an ObjectPtr from a raw pointer
		constexpr ObjectPtr(Archetype* ptr_) noexcept : m_Ptr(ptr_) { }
		/// Construct an ObjectPtr from a raw pointer convertible to T*, such
		/// as BaseOfT*
		template<typename U, typename = std::enable_if_t<std::is_convertible<U*, Archetype*>::value>>
		constexpr ObjectPtr(U* ptr_) noexcept : m_Ptr(ptr_)
		{
		}
		/// Construct an ObjectPtr from a smart pointer that holds a pointer
		/// convertible to T*,
		/// such as shared_ptr<T> or unique_ptr<BaseOfT>
		template<typename Ptr, typename = std::enable_if_t<detail::IsConvertibleSmartPointer<Ptr, Archetype>::value>>
		constexpr ObjectPtr(Ptr const& other_) noexcept : m_Ptr(other_.get())
		{
		}

		/// Get the raw pointer value
		constexpr Archetype* Get() const noexcept
		{
			return m_Ptr;
		}

		/// Dereference the pointer
		constexpr Archetype& operator*() const noexcept
		{
			return *m_Ptr;
		}

		/// Dereference the pointer for ptr->m usage
		constexpr Archetype* operator->() const noexcept
		{
			return m_Ptr;
		}

		/// Allow if(ptr) to test for null
		constexpr explicit operator bool() const noexcept
		{
			return m_Ptr != nullptr;
		}

		/// Convert to a raw pointer where necessary
		constexpr explicit operator Archetype*() const noexcept
		{
			return m_Ptr;
		}

		/// !ptr is true iff ptr is null
		constexpr bool operator!() const noexcept
		{
			return !m_Ptr;
		}

		/// Change the value
		void Reset(Archetype* ptr_ = nullptr) noexcept
		{
			m_Ptr = ptr_;
		}

		/// Check for equality
		friend constexpr bool operator==(ObjectPtr const& lhs_, ObjectPtr const& rhs_) noexcept
		{
			return lhs_.m_Ptr == rhs_.m_Ptr;
		}

		/// Check for inequality
		friend constexpr bool operator!=(ObjectPtr const& lhs_, ObjectPtr const& rhs_) noexcept
		{
			return !(lhs_ == rhs_);
		}

		/// a<b provides a total order
		friend constexpr bool operator<(ObjectPtr const& lhs_, ObjectPtr const& rhs_) noexcept
		{
			return std::less<void>()(lhs_.m_Ptr, rhs_.m_Ptr);
		}
		/// a>b is b<a
		friend constexpr bool operator>(ObjectPtr const& lhs_, ObjectPtr const& rhs_) noexcept
		{
			return rhs_ < lhs_;
		}
		/// a<=b is !(b<a)
		friend constexpr bool operator<=(ObjectPtr const& lhs_, ObjectPtr const& rhs_) noexcept
		{
			return !(rhs_ < lhs_);
		}
		/// a<=b is b<=a
		friend constexpr bool operator>=(ObjectPtr const& lhs_, ObjectPtr const& rhs_) noexcept
		{
			return rhs_ <= lhs_;
		}

	private:
		/// The stored pointer
		Archetype* m_Ptr;
	};

} // namespace jss

namespace std
{
	/// Allow hashing object_ptrs so they can be used as keys in unordered_map
	template<typename Archetype>
	struct hash<jss::ObjectPtr<Archetype>>
	{
		constexpr size_t operator()(jss::ObjectPtr<Archetype> const& p_) const noexcept
		{
			return hash<Archetype*>()(p_.get());
		}
	};

	/// Do a static_cast with ObjectPtr
	template<typename To, typename From>
	typename std::enable_if<sizeof(decltype(static_cast<To*>(std::declval<From*>()))) != 0, jss::ObjectPtr<To>>::type
	StaticPointerCast(jss::ObjectPtr<From> p_)
	{
		return static_cast<To*>(p_.get());
	}

	/// Do a dynamic_cast with ObjectPtr
	template<typename To, typename From>
	typename std::enable_if<sizeof(decltype(dynamic_cast<To*>(std::declval<From*>()))) != 0, jss::ObjectPtr<To>>::type
	DynamicPointerCast(jss::ObjectPtr<From> p_)
	{
		return dynamic_cast<To*>(p_.get());
	}
} // namespace std
