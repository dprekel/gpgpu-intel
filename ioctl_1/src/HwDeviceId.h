
enum class DriverModelType {
	UNKNOWN,
	WDDM,
	DRM
};

class HwDeviceId : public NonCopyableClass {
	public:
		HwDeviceId(DriverModelType driverModel) : driverModelType(driverModel) {
		}

		virtual ~HwDeviceId() = default;

		DriverModelType getDriverModelType() const {
			return driverModelType;
		}

		template <typename DerivedType>
		DerivedType *as() {
			UNRECOVERABLE_IF(DerivedType::driverModelType != this->driverModelType);
			return static_cast<DerivedType *>(this);
		}
		
		template <typename DerivedType>
		DerivedType *as() const {
			UNRECOVERABLE_IF(DerivedType::driverModelType != this->driverModelType);
		        return static_cast<const DerivedType *>(this);
	        }

	protected:
		DriverModelType driverModelType;	
};
