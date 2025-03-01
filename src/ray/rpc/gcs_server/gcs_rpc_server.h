// Copyright 2017 The Ray Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "ray/common/asio/instrumented_io_context.h"
#include "ray/common/id.h"
#include "ray/rpc/grpc_server.h"
#include "ray/rpc/server_call.h"
#include "src/ray/protobuf/gcs_service.grpc.pb.h"

namespace ray {
namespace rpc {

#define JOB_INFO_SERVICE_RPC_HANDLER(HANDLER)     \
  RPC_SERVICE_HANDLER(JobInfoGcsService, HANDLER, \
                      RayConfig::instance().gcs_max_active_rpcs_per_handler())

#define ACTOR_INFO_SERVICE_RPC_HANDLER(HANDLER, MAX_ACTIVE_RPCS) \
  RPC_SERVICE_HANDLER(ActorInfoGcsService, HANDLER, MAX_ACTIVE_RPCS)

#define NODE_INFO_SERVICE_RPC_HANDLER(HANDLER)     \
  RPC_SERVICE_HANDLER(NodeInfoGcsService, HANDLER, \
                      RayConfig::instance().gcs_max_active_rpcs_per_handler())

#define HEARTBEAT_INFO_SERVICE_RPC_HANDLER(HANDLER) \
  RPC_SERVICE_HANDLER(HeartbeatInfoGcsService, HANDLER, -1)

#define NODE_RESOURCE_INFO_SERVICE_RPC_HANDLER(HANDLER)    \
  RPC_SERVICE_HANDLER(NodeResourceInfoGcsService, HANDLER, \
                      RayConfig::instance().gcs_max_active_rpcs_per_handler())

#define OBJECT_INFO_SERVICE_RPC_HANDLER(HANDLER)     \
  RPC_SERVICE_HANDLER(ObjectInfoGcsService, HANDLER, \
                      RayConfig::instance().gcs_max_active_rpcs_per_handler())

#define STATS_SERVICE_RPC_HANDLER(HANDLER)      \
  RPC_SERVICE_HANDLER(StatsGcsService, HANDLER, \
                      RayConfig::instance().gcs_max_active_rpcs_per_handler())

#define WORKER_INFO_SERVICE_RPC_HANDLER(HANDLER)     \
  RPC_SERVICE_HANDLER(WorkerInfoGcsService, HANDLER, \
                      RayConfig::instance().gcs_max_active_rpcs_per_handler())

#define PLACEMENT_GROUP_INFO_SERVICE_RPC_HANDLER(HANDLER)    \
  RPC_SERVICE_HANDLER(PlacementGroupInfoGcsService, HANDLER, \
                      RayConfig::instance().gcs_max_active_rpcs_per_handler())

#define INTERNAL_KV_SERVICE_RPC_HANDLER(HANDLER) \
  RPC_SERVICE_HANDLER(InternalKVGcsService, HANDLER, -1)

// Unlimited max active RPCs, because of long poll.
#define INTERNAL_PUBSUB_SERVICE_RPC_HANDLER(HANDLER) \
  RPC_SERVICE_HANDLER(InternalPubSubGcsService, HANDLER, -1)

#define GCS_RPC_SEND_REPLY(send_reply_callback, reply, status) \
  reply->mutable_status()->set_code((int)status.code());       \
  reply->mutable_status()->set_message(status.message());      \
  send_reply_callback(ray::Status::OK(), nullptr, nullptr)

class JobInfoGcsServiceHandler {
 public:
  virtual ~JobInfoGcsServiceHandler() = default;

  virtual void HandleAddJob(const AddJobRequest &request, AddJobReply *reply,
                            SendReplyCallback send_reply_callback) = 0;

  virtual void HandleMarkJobFinished(const MarkJobFinishedRequest &request,
                                     MarkJobFinishedReply *reply,
                                     SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetAllJobInfo(const GetAllJobInfoRequest &request,
                                   GetAllJobInfoReply *reply,
                                   SendReplyCallback send_reply_callback) = 0;

  virtual void AddJobFinishedListener(
      std::function<void(std::shared_ptr<JobID>)> listener) = 0;

  virtual void HandleReportJobError(const ReportJobErrorRequest &request,
                                    ReportJobErrorReply *reply,
                                    SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetNextJobID(const GetNextJobIDRequest &request,
                                  GetNextJobIDReply *reply,
                                  SendReplyCallback send_reply_callback) = 0;
};

/// The `GrpcService` for `JobInfoGcsService`.
class JobInfoGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] handler The service handler that actually handle the requests.
  explicit JobInfoGrpcService(instrumented_io_context &io_service,
                              JobInfoGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler){};

 protected:
  grpc::Service &GetGrpcService() override { return service_; }

  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    JOB_INFO_SERVICE_RPC_HANDLER(AddJob);
    JOB_INFO_SERVICE_RPC_HANDLER(MarkJobFinished);
    JOB_INFO_SERVICE_RPC_HANDLER(GetAllJobInfo);
    JOB_INFO_SERVICE_RPC_HANDLER(ReportJobError);
    JOB_INFO_SERVICE_RPC_HANDLER(GetNextJobID);
  }

 private:
  /// The grpc async service object.
  JobInfoGcsService::AsyncService service_;
  /// The service handler that actually handle the requests.
  JobInfoGcsServiceHandler &service_handler_;
};

class ActorInfoGcsServiceHandler {
 public:
  virtual ~ActorInfoGcsServiceHandler() = default;

  virtual void HandleRegisterActor(const RegisterActorRequest &request,
                                   RegisterActorReply *reply,
                                   SendReplyCallback send_reply_callback) = 0;

  virtual void HandleCreateActor(const CreateActorRequest &request,
                                 CreateActorReply *reply,
                                 SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetActorInfo(const GetActorInfoRequest &request,
                                  GetActorInfoReply *reply,
                                  SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetNamedActorInfo(const GetNamedActorInfoRequest &request,
                                       GetNamedActorInfoReply *reply,
                                       SendReplyCallback send_reply_callback) = 0;

  virtual void HandleListNamedActors(const rpc::ListNamedActorsRequest &request,
                                     rpc::ListNamedActorsReply *reply,
                                     rpc::SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetAllActorInfo(const GetAllActorInfoRequest &request,
                                     GetAllActorInfoReply *reply,
                                     SendReplyCallback send_reply_callback) = 0;

  virtual void HandleKillActorViaGcs(const KillActorViaGcsRequest &request,
                                     KillActorViaGcsReply *reply,
                                     SendReplyCallback send_reply_callback) = 0;
};

/// The `GrpcService` for `ActorInfoGcsService`.
class ActorInfoGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] handler The service handler that actually handle the requests.
  explicit ActorInfoGrpcService(instrumented_io_context &io_service,
                                ActorInfoGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler){};

 protected:
  grpc::Service &GetGrpcService() override { return service_; }

  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    /// Register/Create Actor RPC takes long time, we shouldn't limit them to avoid
    /// distributed deadlock.
    ACTOR_INFO_SERVICE_RPC_HANDLER(RegisterActor, -1);
    ACTOR_INFO_SERVICE_RPC_HANDLER(CreateActor, -1);

    /// Others need back pressure.
    ACTOR_INFO_SERVICE_RPC_HANDLER(
        GetActorInfo, RayConfig::instance().gcs_max_active_rpcs_per_handler());
    ACTOR_INFO_SERVICE_RPC_HANDLER(
        GetNamedActorInfo, RayConfig::instance().gcs_max_active_rpcs_per_handler());
    ACTOR_INFO_SERVICE_RPC_HANDLER(
        ListNamedActors, RayConfig::instance().gcs_max_active_rpcs_per_handler());
    ACTOR_INFO_SERVICE_RPC_HANDLER(
        GetAllActorInfo, RayConfig::instance().gcs_max_active_rpcs_per_handler());
    ACTOR_INFO_SERVICE_RPC_HANDLER(
        KillActorViaGcs, RayConfig::instance().gcs_max_active_rpcs_per_handler());
  }

 private:
  /// The grpc async service object.
  ActorInfoGcsService::AsyncService service_;
  /// The service handler that actually handle the requests.
  ActorInfoGcsServiceHandler &service_handler_;
};

class NodeInfoGcsServiceHandler {
 public:
  virtual ~NodeInfoGcsServiceHandler() = default;

  virtual void HandleRegisterNode(const RegisterNodeRequest &request,
                                  RegisterNodeReply *reply,
                                  SendReplyCallback send_reply_callback) = 0;

  virtual void HandleDrainNode(const DrainNodeRequest &request, DrainNodeReply *reply,
                               SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetAllNodeInfo(const GetAllNodeInfoRequest &request,
                                    GetAllNodeInfoReply *reply,
                                    SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetInternalConfig(const GetInternalConfigRequest &request,
                                       GetInternalConfigReply *reply,
                                       SendReplyCallback send_reply_callback) = 0;
};

/// The `GrpcService` for `NodeInfoGcsService`.
class NodeInfoGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] handler The service handler that actually handle the requests.
  explicit NodeInfoGrpcService(instrumented_io_context &io_service,
                               NodeInfoGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler){};

 protected:
  grpc::Service &GetGrpcService() override { return service_; }

  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    NODE_INFO_SERVICE_RPC_HANDLER(RegisterNode);
    NODE_INFO_SERVICE_RPC_HANDLER(DrainNode);
    NODE_INFO_SERVICE_RPC_HANDLER(GetAllNodeInfo);
    NODE_INFO_SERVICE_RPC_HANDLER(GetInternalConfig);
  }

 private:
  /// The grpc async service object.
  NodeInfoGcsService::AsyncService service_;
  /// The service handler that actually handle the requests.
  NodeInfoGcsServiceHandler &service_handler_;
};

class NodeResourceInfoGcsServiceHandler {
 public:
  virtual ~NodeResourceInfoGcsServiceHandler() = default;

  virtual void HandleGetResources(const GetResourcesRequest &request,
                                  GetResourcesReply *reply,
                                  SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetAllAvailableResources(
      const rpc::GetAllAvailableResourcesRequest &request,
      rpc::GetAllAvailableResourcesReply *reply,
      rpc::SendReplyCallback send_reply_callback) = 0;

  virtual void HandleReportResourceUsage(const ReportResourceUsageRequest &request,
                                         ReportResourceUsageReply *reply,
                                         SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetAllResourceUsage(const GetAllResourceUsageRequest &request,
                                         GetAllResourceUsageReply *reply,
                                         SendReplyCallback send_reply_callback) = 0;
};

/// The `GrpcService` for `NodeResourceInfoGcsService`.
class NodeResourceInfoGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] handler The service handler that actually handle the requests.
  explicit NodeResourceInfoGrpcService(instrumented_io_context &io_service,
                                       NodeResourceInfoGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler){};

 protected:
  grpc::Service &GetGrpcService() override { return service_; }

  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    NODE_RESOURCE_INFO_SERVICE_RPC_HANDLER(GetResources);
    NODE_RESOURCE_INFO_SERVICE_RPC_HANDLER(GetAllAvailableResources);
    NODE_RESOURCE_INFO_SERVICE_RPC_HANDLER(ReportResourceUsage);
    NODE_RESOURCE_INFO_SERVICE_RPC_HANDLER(GetAllResourceUsage);
  }

 private:
  /// The grpc async service object.
  NodeResourceInfoGcsService::AsyncService service_;
  /// The service handler that actually handle the requests.
  NodeResourceInfoGcsServiceHandler &service_handler_;
};

class HeartbeatInfoGcsServiceHandler {
 public:
  virtual ~HeartbeatInfoGcsServiceHandler() = default;
  virtual void HandleReportHeartbeat(const ReportHeartbeatRequest &request,
                                     ReportHeartbeatReply *reply,
                                     SendReplyCallback send_reply_callback) = 0;
  virtual void HandleCheckAlive(const CheckAliveRequest &request, CheckAliveReply *reply,
                                SendReplyCallback send_reply_callback) = 0;
};
/// The `GrpcService` for `HeartbeatInfoGcsService`.
class HeartbeatInfoGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] handler The service handler that actually handle the requests.
  explicit HeartbeatInfoGrpcService(instrumented_io_context &io_service,
                                    HeartbeatInfoGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler){};

 protected:
  grpc::Service &GetGrpcService() override { return service_; }
  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    HEARTBEAT_INFO_SERVICE_RPC_HANDLER(ReportHeartbeat);
    HEARTBEAT_INFO_SERVICE_RPC_HANDLER(CheckAlive);
  }

 private:
  /// The grpc async service object.
  HeartbeatInfoGcsService::AsyncService service_;
  /// The service handler that actually handle the requests.
  HeartbeatInfoGcsServiceHandler &service_handler_;
};

class StatsGcsServiceHandler {
 public:
  virtual ~StatsGcsServiceHandler() = default;

  virtual void HandleAddProfileData(const AddProfileDataRequest &request,
                                    AddProfileDataReply *reply,
                                    SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetAllProfileInfo(const GetAllProfileInfoRequest &request,
                                       GetAllProfileInfoReply *reply,
                                       SendReplyCallback send_reply_callback) = 0;
};

/// The `GrpcService` for `StatsGcsService`.
class StatsGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] handler The service handler that actually handle the requests.
  explicit StatsGrpcService(instrumented_io_context &io_service,
                            StatsGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler){};

 protected:
  grpc::Service &GetGrpcService() override { return service_; }

  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    STATS_SERVICE_RPC_HANDLER(AddProfileData);
    STATS_SERVICE_RPC_HANDLER(GetAllProfileInfo);
  }

 private:
  /// The grpc async service object.
  StatsGcsService::AsyncService service_;
  /// The service handler that actually handle the requests.
  StatsGcsServiceHandler &service_handler_;
};

class WorkerInfoGcsServiceHandler {
 public:
  virtual ~WorkerInfoGcsServiceHandler() = default;

  virtual void HandleReportWorkerFailure(const ReportWorkerFailureRequest &request,
                                         ReportWorkerFailureReply *reply,
                                         SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetWorkerInfo(const GetWorkerInfoRequest &request,
                                   GetWorkerInfoReply *reply,
                                   SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetAllWorkerInfo(const GetAllWorkerInfoRequest &request,
                                      GetAllWorkerInfoReply *reply,
                                      SendReplyCallback send_reply_callback) = 0;

  virtual void HandleAddWorkerInfo(const AddWorkerInfoRequest &request,
                                   AddWorkerInfoReply *reply,
                                   SendReplyCallback send_reply_callback) = 0;
};

/// The `GrpcService` for `WorkerInfoGcsService`.
class WorkerInfoGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] handler The service handler that actually handle the requests.
  explicit WorkerInfoGrpcService(instrumented_io_context &io_service,
                                 WorkerInfoGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler){};

 protected:
  grpc::Service &GetGrpcService() override { return service_; }

  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    WORKER_INFO_SERVICE_RPC_HANDLER(ReportWorkerFailure);
    WORKER_INFO_SERVICE_RPC_HANDLER(GetWorkerInfo);
    WORKER_INFO_SERVICE_RPC_HANDLER(GetAllWorkerInfo);
    WORKER_INFO_SERVICE_RPC_HANDLER(AddWorkerInfo);
  }

 private:
  /// The grpc async service object.
  WorkerInfoGcsService::AsyncService service_;
  /// The service handler that actually handle the requests.
  WorkerInfoGcsServiceHandler &service_handler_;
};

class PlacementGroupInfoGcsServiceHandler {
 public:
  virtual ~PlacementGroupInfoGcsServiceHandler() = default;

  virtual void HandleCreatePlacementGroup(const CreatePlacementGroupRequest &request,
                                          CreatePlacementGroupReply *reply,
                                          SendReplyCallback send_reply_callback) = 0;

  virtual void HandleRemovePlacementGroup(const RemovePlacementGroupRequest &request,
                                          RemovePlacementGroupReply *reply,
                                          SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetPlacementGroup(const GetPlacementGroupRequest &request,
                                       GetPlacementGroupReply *reply,
                                       SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetAllPlacementGroup(const GetAllPlacementGroupRequest &request,
                                          GetAllPlacementGroupReply *reply,
                                          SendReplyCallback send_reply_callback) = 0;

  virtual void HandleWaitPlacementGroupUntilReady(
      const WaitPlacementGroupUntilReadyRequest &request,
      WaitPlacementGroupUntilReadyReply *reply,
      SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGetNamedPlacementGroup(const GetNamedPlacementGroupRequest &request,
                                            GetNamedPlacementGroupReply *reply,
                                            SendReplyCallback send_reply_callback) = 0;
};

/// The `GrpcService` for `PlacementGroupInfoGcsService`.
class PlacementGroupInfoGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] handler The service handler that actually handle the requests.
  explicit PlacementGroupInfoGrpcService(instrumented_io_context &io_service,
                                         PlacementGroupInfoGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler) {}

 protected:
  grpc::Service &GetGrpcService() override { return service_; }

  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    PLACEMENT_GROUP_INFO_SERVICE_RPC_HANDLER(CreatePlacementGroup);
    PLACEMENT_GROUP_INFO_SERVICE_RPC_HANDLER(RemovePlacementGroup);
    PLACEMENT_GROUP_INFO_SERVICE_RPC_HANDLER(GetPlacementGroup);
    PLACEMENT_GROUP_INFO_SERVICE_RPC_HANDLER(GetNamedPlacementGroup);
    PLACEMENT_GROUP_INFO_SERVICE_RPC_HANDLER(GetAllPlacementGroup);
    PLACEMENT_GROUP_INFO_SERVICE_RPC_HANDLER(WaitPlacementGroupUntilReady);
  }

 private:
  /// The grpc async service object.
  PlacementGroupInfoGcsService::AsyncService service_;
  /// The service handler that actually handle the requests.
  PlacementGroupInfoGcsServiceHandler &service_handler_;
};

class InternalKVGcsServiceHandler {
 public:
  virtual ~InternalKVGcsServiceHandler() = default;
  virtual void HandleInternalKVKeys(const InternalKVKeysRequest &request,
                                    InternalKVKeysReply *reply,
                                    SendReplyCallback send_reply_callback) = 0;

  virtual void HandleInternalKVGet(const InternalKVGetRequest &request,
                                   InternalKVGetReply *reply,
                                   SendReplyCallback send_reply_callback) = 0;

  virtual void HandleInternalKVPut(const InternalKVPutRequest &request,
                                   InternalKVPutReply *reply,
                                   SendReplyCallback send_reply_callback) = 0;

  virtual void HandleInternalKVDel(const InternalKVDelRequest &request,
                                   InternalKVDelReply *reply,
                                   SendReplyCallback send_reply_callback) = 0;

  virtual void HandleInternalKVExists(const InternalKVExistsRequest &request,
                                      InternalKVExistsReply *reply,
                                      SendReplyCallback send_reply_callback) = 0;
};

class InternalKVGrpcService : public GrpcService {
 public:
  explicit InternalKVGrpcService(instrumented_io_context &io_service,
                                 InternalKVGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler) {}

 protected:
  grpc::Service &GetGrpcService() override { return service_; }
  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    INTERNAL_KV_SERVICE_RPC_HANDLER(InternalKVGet);
    INTERNAL_KV_SERVICE_RPC_HANDLER(InternalKVPut);
    INTERNAL_KV_SERVICE_RPC_HANDLER(InternalKVDel);
    INTERNAL_KV_SERVICE_RPC_HANDLER(InternalKVExists);
    INTERNAL_KV_SERVICE_RPC_HANDLER(InternalKVKeys);
  }

 private:
  InternalKVGcsService::AsyncService service_;
  InternalKVGcsServiceHandler &service_handler_;
};

class InternalPubSubGcsServiceHandler {
 public:
  virtual ~InternalPubSubGcsServiceHandler() = default;

  virtual void HandleGcsPublish(const GcsPublishRequest &request, GcsPublishReply *reply,
                                SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGcsSubscriberPoll(const GcsSubscriberPollRequest &request,
                                       GcsSubscriberPollReply *reply,
                                       SendReplyCallback send_reply_callback) = 0;

  virtual void HandleGcsSubscriberCommandBatch(
      const GcsSubscriberCommandBatchRequest &request,
      GcsSubscriberCommandBatchReply *reply, SendReplyCallback send_reply_callback) = 0;
};

class InternalPubSubGrpcService : public GrpcService {
 public:
  InternalPubSubGrpcService(instrumented_io_context &io_service,
                            InternalPubSubGcsServiceHandler &handler)
      : GrpcService(io_service), service_handler_(handler) {}

 protected:
  grpc::Service &GetGrpcService() override { return service_; }
  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    INTERNAL_PUBSUB_SERVICE_RPC_HANDLER(GcsPublish);
    INTERNAL_PUBSUB_SERVICE_RPC_HANDLER(GcsSubscriberPoll);
    INTERNAL_PUBSUB_SERVICE_RPC_HANDLER(GcsSubscriberCommandBatch);
  }

 private:
  InternalPubSubGcsService::AsyncService service_;
  InternalPubSubGcsServiceHandler &service_handler_;
};

using JobInfoHandler = JobInfoGcsServiceHandler;
using ActorInfoHandler = ActorInfoGcsServiceHandler;
using NodeInfoHandler = NodeInfoGcsServiceHandler;
using NodeResourceInfoHandler = NodeResourceInfoGcsServiceHandler;
using HeartbeatInfoHandler = HeartbeatInfoGcsServiceHandler;
using StatsHandler = StatsGcsServiceHandler;
using WorkerInfoHandler = WorkerInfoGcsServiceHandler;
using PlacementGroupInfoHandler = PlacementGroupInfoGcsServiceHandler;
using InternalKVHandler = InternalKVGcsServiceHandler;
using InternalPubSubHandler = InternalPubSubGcsServiceHandler;

}  // namespace rpc
}  // namespace ray
