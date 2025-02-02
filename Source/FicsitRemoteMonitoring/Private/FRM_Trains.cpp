// Fill out your copyright notice in the Description page of Project Settings.


#include "FRM_Trains.h"

TArray<TSharedPtr<FJsonValue>> UFRM_Trains::getTrains(UObject* WorldContext) {

	AFGRailroadSubsystem* RailroadSubsystem = AFGRailroadSubsystem::Get(WorldContext->GetWorld());
	
	TArray<AFGTrain*> Trains;
	RailroadSubsystem->GetAllTrains(Trains);
	TArray<TSharedPtr<FJsonValue>> JTrainsArray;

	for (AFGTrain* Train : Trains) {

		TSharedPtr<FJsonObject> JTrain = MakeShared<FJsonObject>();
		TArray<TSharedPtr<FJsonValue>> JTimetableArray;

		AFGLocomotive* MasterTrain = Train->GetMultipleUnitMaster();

		float TTotalMass = 0.0;
		float TPayloadMass = 0.0;
		float TMaxPayloadMass = 0.0;
		float ForwardSpeed = 0.0;
		float ThrottlePercent = 0.0;
		FString TrainStation = TEXT("No Station");

		//UE_LOG(LogFRMAPI, Log, TEXT("Train Unit: %s - Prior to multi-master"), *FString(Train->GetTrainName().ToString()));

		AFGLocomotive* MultiUnitMaster = Train->GetMultipleUnitMaster();

		//UE_LOG(LogFRMAPI, Log, TEXT("MultiMaster Valid: %s"), IsValid(MultiUnitMaster) ? TEXT("true") : TEXT("false"));

		TArray<TSharedPtr<FJsonValue>> JPlayerArray;
		TArray<TSharedPtr<FJsonValue>> JRailcarsArray;
		TSharedPtr<FJsonObject> TrainLocation;

		UFGPowerInfoComponent* PowerInfo = NULL;
		if (IsValid(MultiUnitMaster)) {

			AFGRailroadTimeTable* TimeTable = Train->GetTimeTable();
			FTimeTableStop CurrentStop;

			TrainLocation = UFRM_Library::getActorJSON(MultiUnitMaster);

			TArray<FTimeTableStop> TrainStops;

			if (IsValid(TimeTable)) {
				int32 StopIndex = TimeTable->GetCurrentStop();
				CurrentStop = TimeTable->GetStop(StopIndex);
				TimeTable->GetStops(TrainStops);
				AFGTrainStationIdentifier* CurrentStation = CurrentStop.Station;

				if (IsValid(CurrentStation)) {
					TrainStation = CurrentStation->GetStationName().ToString();
				}
			}

			UFGLocomotiveMovementComponent* LocomotiveMovement = MultiUnitMaster->GetLocomotiveMovementComponent();

			for (FTimeTableStop TrainStop : TrainStops) {
				TSharedPtr<FJsonObject> JTimetable = MakeShared<FJsonObject>();
				JTimetable->Values.Add("StationName", MakeShared<FJsonValueString>(TrainStop.Station->GetStationName().ToString()));
				JTimetableArray.Add(MakeShared<FJsonValueObject>(JTimetable));
			};

			ForwardSpeed = LocomotiveMovement->GetForwardSpeed();
			ThrottlePercent = LocomotiveMovement->GetThrottle() * 100;
			PowerInfo = Train->GetMultipleUnitMaster()->GetPowerInfo();
			
		} else {

			TrainLocation = UFRM_Library::getActorJSON(Train);

		}

		TArray<AFGRailroadVehicle*> Railcars = Train->mConsistData.Vehicles;

		for (AFGRailroadVehicle* Railcar : Railcars) {

			TSharedPtr<FJsonObject> JRailcars = MakeShared<FJsonObject>();

			UFGRailroadVehicleMovementComponent* RailcarVehicleMovement = Railcar->GetRailroadVehicleMovementComponent();

			JRailcars->Values.Add("Name", MakeShared<FJsonValueString>(Railcar->mDisplayName.ToString()));
			JRailcars->Values.Add("ClassName", MakeShared<FJsonValueString>(Railcar->GetClass()->GetName()));
			JRailcars->Values.Add("TotalMass", MakeShared<FJsonValueNumber>(RailcarVehicleMovement->GetMass()));
			JRailcars->Values.Add("PayloadMass", MakeShared<FJsonValueNumber>(RailcarVehicleMovement->GetPayloadMass()));
			JRailcars->Values.Add("MaxPayloadMass", MakeShared<FJsonValueNumber>(RailcarVehicleMovement->GetMaxPayloadMass()));

			// test if Railcar is a FreightWagon and get the inventory
			if (AFGFreightWagon* FreightWagon = Cast<AFGFreightWagon>(Railcar)) {
				TArray<FInventoryStack> InventoryStacks;
				FreightWagon->GetFreightInventory()->GetInventoryStacks(InventoryStacks);
				TMap<TSubclassOf<UFGItemDescriptor>, int32> FreightInventory = UFRM_Library::GetGroupedInventoryItems(InventoryStacks);
				JRailcars->Values.Add("Inventory", MakeShared<FJsonValueArray>(UFRM_Library::GetInventoryJSON(FreightInventory)));
			}
			// otherwise create an empty inventory (for locomotives)
			else {
				JRailcars->Values.Add("Inventory", MakeShared<FJsonValueArray>(TArray<TSharedPtr<FJsonValue>>()));
			}

			TTotalMass = TTotalMass + RailcarVehicleMovement->GetMass();
			TPayloadMass = TPayloadMass + RailcarVehicleMovement->GetPayloadMass();
			TMaxPayloadMass = TMaxPayloadMass + RailcarVehicleMovement->GetMaxPayloadMass();

			JRailcarsArray.Add(MakeShared<FJsonValueObject>(JRailcars));
		};

		FString FormString = "Unknown";
		switch (Train->GetTrainStatus()) {
			case ETrainStatus::TS_Parked :			FormString = "Parked";
				break;
			case ETrainStatus::TS_ManualDriving:	FormString = "Manual Driving";
				break;
			case ETrainStatus::TS_SelfDriving:		FormString = "Self-Driving";
				break;
			case ETrainStatus::TS_Derailed:			FormString = "Derailed";
		};

		JTrain->Values.Add("Name", MakeShared<FJsonValueString>(Train->GetTrainName().ToString()));
		JTrain->Values.Add("ClassName", MakeShared<FJsonValueString>(Train->GetClass()->GetName()));
		JTrain->Values.Add("location", MakeShared<FJsonValueObject>(TrainLocation));
		JTrain->Values.Add("TotalMass", MakeShared<FJsonValueNumber>(TTotalMass));
		JTrain->Values.Add("PayloadMass", MakeShared<FJsonValueNumber>(TPayloadMass));
		JTrain->Values.Add("MaxPayloadMass", MakeShared<FJsonValueNumber>(TMaxPayloadMass));
		JTrain->Values.Add("ForwardSpeed", MakeShared<FJsonValueNumber>(ForwardSpeed));
		JTrain->Values.Add("ThrottlePercent", MakeShared<FJsonValueNumber>(ThrottlePercent));
		JTrain->Values.Add("TrainStation", MakeShared<FJsonValueString>(TrainStation));
		JTrain->Values.Add("Derailed", MakeShared<FJsonValueBoolean>(Train->IsDerailed()));
		JTrain->Values.Add("PendingDerail", MakeShared<FJsonValueBoolean>(Train->HasPendingCollision()));
		JTrain->Values.Add("Status", MakeShared<FJsonValueString>(FormString));
		JTrain->Values.Add("TimeTable", MakeShared<FJsonValueArray>(JTimetableArray));
		JTrain->Values.Add("Vehicles", MakeShared<FJsonValueArray>(JRailcarsArray));
		JTrain->Values.Add("features", MakeShared<FJsonValueObject>(UFRM_Library::getActorFeaturesJSON(Train, Train->GetTrainName().ToString(), "Train")));
		JTrain->Values.Add("PowerInfo", MakeShared<FJsonValueObject>(UFRM_Library::getPowerConsumptionJSON(PowerInfo)));

		JTrainsArray.Add(MakeShared<FJsonValueObject>(JTrain));

	};

	return JTrainsArray;

};

TArray<TSharedPtr<FJsonValue>> UFRM_Trains::getTrainStation(UObject* WorldContext) {

	AFGRailroadSubsystem* RailroadSubsystem = AFGRailroadSubsystem::Get(WorldContext->GetWorld());
	TArray<TSharedPtr<FJsonValue>> JTrainStationArray;
	TArray<AFGTrainStationIdentifier*> TrainStations;
	RailroadSubsystem->GetAllTrainStations(TrainStations);
	
	for (AFGTrainStationIdentifier* TrainStation : TrainStations) {

		float TransferRate = 0;
		float InFlowRate = 0;
		float OutFlowRate = 0;
		
		TSharedPtr<FJsonObject> JTrainStation = MakeShared<FJsonObject>();
		TArray<TSharedPtr<FJsonValue>> JCargoInventory;
		TArray<TSharedPtr<FJsonValue>> JCargoStations;

		AFGBuildableRailroadStation* RailStation = TrainStation->GetStation();
		
		UFGTrainPlatformConnection* StationConnection = RailStation->GetStationOutputConnection();
		UFGTrainPlatformConnection* TrainPlatformConnection = StationConnection->GetConnectedTo();
		AFGBuildableTrainPlatform* TrainPlatform = nullptr;
		
		if (TrainPlatformConnection)
		{
			 TrainPlatform = TrainPlatformConnection->GetPlatformOwner();
		}
				
		TArray<TSharedPtr<FJsonValue>> JTrainPlatformArray;

		bool bCompleted = false;
		
		while (!bCompleted) {	
			AFGBuildableTrainPlatformCargo* TrainPlatformCargo = Cast<AFGBuildableTrainPlatformCargo>(TrainPlatform);
			TSharedPtr<FJsonObject> JTrainPlatform = MakeShared<FJsonObject>();
			TMap<TSubclassOf<UFGItemDescriptor>, int32> TrainPlatformInventory;
			
			// Skips but does not stop checking for AFGBuildableTrainPlatformCargo
			if (Cast<AFGBuildableTrainPlatformEmpty>(TrainPlatform))
			{
				JTrainPlatform->Values.Add("Name", MakeShared<FJsonValueString>(TrainPlatform->mDisplayName.ToString()));
				JTrainPlatform->Values.Add("ClassName", MakeShared<FJsonValueString>(UKismetSystemLibrary::GetClassDisplayName(TrainPlatform->GetClass())));
				JTrainPlatform->Values.Add("location", MakeShared<FJsonValueObject>(UFRM_Library::getActorJSON(TrainPlatform)));
				JTrainPlatform->Values.Add("PowerInfo", MakeShared<FJsonValueObject>(UFRM_Library::getPowerConsumptionJSON(TrainPlatform->GetPowerInfo())));
				JTrainPlatform->Values.Add("TransferRate", MakeShared<FJsonValueNumber>(0));
				JTrainPlatform->Values.Add("InflowRate", MakeShared<FJsonValueNumber>(0));
				JTrainPlatform->Values.Add("OutflowRate", MakeShared<FJsonValueNumber>(0));
				JTrainPlatform->Values.Add("LoadingMode", MakeShared<FJsonValueString>("Empty Platform"));
				JTrainPlatform->Values.Add("LoadingStatus", MakeShared<FJsonValueString>("Empty Platform"));
				JTrainPlatform->Values.Add("DockingStatus", MakeShared<FJsonValueString>("Empty Platform"));
				JTrainPlatform->Values.Add("Inventory", MakeShared<FJsonValueArray>(UFRM_Library::GetInventoryJSON(TrainPlatformInventory)));

				JTrainPlatformArray.Add(MakeShared<FJsonValueObject>(JTrainPlatform));
				
				UFGTrainPlatformConnection* TrainPlatformConnectionBuffer = TrainPlatform->GetConnectionInOppositeDirection(TrainPlatformConnection);
				TrainPlatformConnection = TrainPlatformConnectionBuffer->GetConnectedTo();

				if (TrainPlatformConnection)
				{
					TrainPlatform = TrainPlatformConnection->GetPlatformOwner();
				} else
				{
					//No valid connection, we're done here.
					bCompleted = true;
					break;
				}
				continue;
			}
			
			if (!TrainPlatformCargo) {
				bCompleted = true;
				continue;
			}
			
			float CargoTransferRate = 0;
			float CargoInFlowRate = 0;
			float CargoOutFlowRate = 0;

			CargoTransferRate = TrainPlatformCargo->GetCurrentItemTransferRate();
			CargoInFlowRate = TrainPlatformCargo->GetInflowRate();
			CargoOutFlowRate = TrainPlatformCargo->GetOutflowRate();

			TransferRate = TransferRate + CargoTransferRate;
			InFlowRate = InFlowRate + CargoInFlowRate;
			OutFlowRate = OutFlowRate + CargoOutFlowRate;

			FString LoadMode = TEXT("Unloading");
			if (TrainPlatformCargo->GetIsInLoadMode()) { LoadMode = TEXT("Loading"); }

			FString LoadingStatus = TEXT("Idle");
			if (TrainPlatformCargo->IsLoadUnloading()) { LoadingStatus = LoadMode; }

			FString StatusString = "";
			ETrainPlatformDockingStatus DockingStatus = TrainPlatformCargo->GetDockingStatus();

			switch (DockingStatus)
			{
				case ETrainPlatformDockingStatus::ETPDS_Complete					:	StatusString = TEXT("Complete");
					break;
				case ETrainPlatformDockingStatus::ETPDS_IdleWaitForTime				:	StatusString = TEXT("Idle Wait For Time");
					break;
				case ETrainPlatformDockingStatus::ETPDS_Loading						:	StatusString = TEXT("Loading");
					break;
				case ETrainPlatformDockingStatus::ETPDS_None						:	StatusString = TEXT("None");
					break;
				case ETrainPlatformDockingStatus::ETPDS_Unloading					:	StatusString = TEXT("Unloading");
					break;
				case ETrainPlatformDockingStatus::ETPDS_WaitForTransferCondition	:	StatusString = TEXT("Wait for Transfer Condition");
					break;
				case ETrainPlatformDockingStatus::ETPDS_WaitingForTransfer			:	StatusString = TEXT("Waiting For Transfer");
					break;
				case ETrainPlatformDockingStatus::ETPDS_WaitingToStart				:	StatusString = TEXT("Waiting To Start");
					break;
			}

			// get train platform inventory
			TrainPlatformInventory = UFRM_Library::GetGroupedInventoryItems(TrainPlatformCargo->GetInventory());

			JTrainPlatform->Values.Add("Name", MakeShared<FJsonValueString>(TrainPlatformCargo->mDisplayName.ToString()));
			JTrainPlatform->Values.Add("ClassName", MakeShared<FJsonValueString>(UKismetSystemLibrary::GetClassDisplayName(TrainPlatformCargo->GetClass())));
			JTrainPlatform->Values.Add("location", MakeShared<FJsonValueObject>(UFRM_Library::getActorJSON(TrainPlatformCargo)));
			JTrainPlatform->Values.Add("PowerInfo", MakeShared<FJsonValueObject>(UFRM_Library::getPowerConsumptionJSON(TrainPlatformCargo->GetPowerInfo())));
			JTrainPlatform->Values.Add("TransferRate", MakeShared<FJsonValueNumber>(CargoTransferRate));
			JTrainPlatform->Values.Add("InflowRate", MakeShared<FJsonValueNumber>(CargoInFlowRate));
			JTrainPlatform->Values.Add("OutflowRate", MakeShared<FJsonValueNumber>(CargoOutFlowRate));
			JTrainPlatform->Values.Add("LoadingMode", MakeShared<FJsonValueString>(LoadMode));
			JTrainPlatform->Values.Add("LoadingStatus", MakeShared<FJsonValueString>(LoadingStatus));
			JTrainPlatform->Values.Add("DockingStatus", MakeShared<FJsonValueString>(StatusString));
			JTrainPlatform->Values.Add("Inventory", MakeShared<FJsonValueArray>(UFRM_Library::GetInventoryJSON(TrainPlatformInventory)));

			JTrainPlatformArray.Add(MakeShared<FJsonValueObject>(JTrainPlatform));

			UFGTrainPlatformConnection* TrainPlatformConnectionBuffer = TrainPlatform->GetConnectionInOppositeDirection(TrainPlatformConnection);
			TrainPlatformConnection = TrainPlatformConnectionBuffer->GetConnectedTo();

			if (IsValid(TrainPlatformConnection))
			{
				TrainPlatform = TrainPlatformConnection->GetPlatformOwner();
			} else
			{
				//No valid connection, we're done here.
				bCompleted = true;
				break;
			}
			
		}

		JTrainStation->Values.Add("Name", MakeShared<FJsonValueString>(TrainStation->GetStationName().ToString()));
		JTrainStation->Values.Add("ClassName", MakeShared<FJsonValueString>(UKismetSystemLibrary::GetClassDisplayName(RailStation->GetClass())));
		JTrainStation->Values.Add("location", MakeShared<FJsonValueObject>(UFRM_Library::getActorJSON(TrainStation)));
		JTrainStation->Values.Add("TransferRate", MakeShared<FJsonValueNumber>(TransferRate));
		JTrainStation->Values.Add("InflowRate", MakeShared<FJsonValueNumber>(InFlowRate));
		JTrainStation->Values.Add("OutflowRate", MakeShared<FJsonValueNumber>(OutFlowRate));
		JTrainStation->Values.Add("CargoInventory", MakeShared<FJsonValueArray>(JTrainPlatformArray));
		JTrainStation->Values.Add("features", MakeShared<FJsonValueObject>(UFRM_Library::getActorFeaturesJSON(TrainStation, TrainStation->GetStationName().ToString(), TEXT("Train Station"))));
		JTrainStation->Values.Add("PowerInfo", MakeShared<FJsonValueObject>(UFRM_Library::getPowerConsumptionJSON(TrainStation->GetStation()->GetPowerInfo())));

		JTrainStationArray.Add(MakeShared<FJsonValueObject>(JTrainStation));
	};

	return JTrainStationArray;
};
